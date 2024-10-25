#include "customer.h"
#include "/home/mubashir/banking/server/map.h"
#include <pthread.h>
#include <sys/stat.h>
extern struct user_map *cust;
#define BUFFER_SIZE 1024
void Customer_functionalities(const int client_socket, struct user_map *ptr)
{
    printf("Customer Func\n");
    fflush(stdout);
    while (1)
    {
        char rbuffer[BUFFER_SIZE];
        char wbuffer[BUFFER_SIZE];
        char *menu = "1) View Account Balance\n"
                     "2) Deposit Money\n"
                     "3) Withdraw Money\n"
                     "4) Transfer Funds\n"
                     "5) Apply for a Loan\n"
                     "6) Change Password\n"
                     "7) Adding Feedback\n"
                     "8) View Transaction History\n"
                     "9) Logout\n"
                     "10) Exit\n";

        send(client_socket, menu, strlen(menu), 0);
        sleep(2);

        struct service selected_service;
        recv(client_socket, &selected_service, sizeof(struct service), 0);
        struct customer record;
        if (selected_service.service_type == 1)
        {
            int fd = open("/home/mubashir/banking/db/customer", O_RDONLY);
            acquire_read_lock(fd, sizeof(struct customer) * ptr->index, sizeof(struct customer));
            lseek(fd, sizeof(struct customer) * ptr->index, SEEK_SET);
            read(fd, &record, sizeof(struct customer));
            release_lock(fd, sizeof(struct customer) * ptr->index, sizeof(struct customer));
            sprintf(wbuffer, "%d", record.balance);
            send(client_socket, wbuffer, BUFFER_SIZE, 0);
            close(fd);
        }
        else if (selected_service.service_type == 2)
        {
            struct amount msg;
            recv(client_socket, &msg, sizeof(struct amount), 0);
            int fd = open("/home/mubashir/banking/db/customer", O_RDWR);
            acquire_write_lock(fd, sizeof(struct customer) * ptr->index, sizeof(struct customer));
            lseek(fd, sizeof(struct customer) * ptr->index, SEEK_SET);
            read(fd, &record, sizeof(struct customer));
            record.balance += msg.value;
            lseek(fd, -sizeof(record), SEEK_CUR);
            write(fd, &record, sizeof(struct customer));
            release_lock(fd, sizeof(struct customer) * ptr->index, sizeof(struct customer));
            close(fd);
            strcpy(wbuffer, "amount deposited Successfully");
            send(client_socket, wbuffer, BUFFER_SIZE, 0);
        }
        else if (selected_service.service_type == 3)
        {
            struct amount msg;
            recv(client_socket, &msg, sizeof(struct amount), 0);
            int fd = open("/home/mubashir/banking/db/customer", O_RDWR);
            acquire_write_lock(fd, sizeof(struct customer) * ptr->index, sizeof(struct customer));
            lseek(fd, sizeof(struct customer) * ptr->index, SEEK_SET);
            read(fd, &record, sizeof(struct customer));
            if (record.balance < msg.value)
            {
                strcpy(wbuffer, "Can't Withdraw");
                send(client_socket, wbuffer, BUFFER_SIZE, 0);
            }
            else
            {
                record.balance -= msg.value;
                lseek(fd, -sizeof(record), SEEK_CUR);
                write(fd, &record, sizeof(struct customer));
                strcpy(wbuffer, "Amount Withdrawn Successfully");
                send(client_socket, wbuffer, BUFFER_SIZE, 0);
            }
            release_lock(fd, sizeof(struct customer) * ptr->index, sizeof(struct customer));
            close(fd);
        }
        else if (selected_service.service_type == 4)
        {
            struct transaction transfer_info;
            struct passbook_entry info;
            recv(client_socket, &transfer_info, sizeof(struct transaction), 0);
            struct user_map *ptr_to_recv = find_user(transfer_info.receiver_username, &cust);
            if (ptr_to_recv == NULL)
            {
                strcpy(wbuffer, "User does not exists");
                send(client_socket, wbuffer, BUFFER_SIZE, 0);
                sleep(1);
            }
            else
            {
                struct customer recv_record;
                int fd = open("/home/mubashir/banking/db/customer", O_RDWR);
                acquire_write_lock(fd, sizeof(struct customer) * ptr->index, sizeof(struct customer));
                acquire_write_lock(fd, sizeof(struct customer) * ptr_to_recv->index, sizeof(struct customer));
                lseek(fd, sizeof(record) * ptr->index, SEEK_SET);
                read(fd, &record, sizeof(struct customer));
                lseek(fd, sizeof(record) * ptr_to_recv->index, SEEK_SET);
                read(fd, &recv_record, sizeof(struct customer));
                if (record.balance < transfer_info.amount)
                {
                    strcpy(wbuffer, "Insufficent Funds:");
                    send(client_socket, wbuffer, BUFFER_SIZE, 0);
                }
                else
                {
                    // UPDATE RECORDS
                    struct passbook_entry s_info;
                    struct passbook_entry r_info;
                    record.balance -= transfer_info.amount;
                    recv_record.balance += transfer_info.amount;

                    // update senders record
                    lseek(fd, sizeof(record) * ptr->index, SEEK_SET);
                    write(fd, &record, sizeof(struct customer));

                    // update receivers record
                    lseek(fd, sizeof(record) * ptr_to_recv->index, SEEK_SET);
                    write(fd, &recv_record, sizeof(struct customer));

                    // UPDATE PASSBOOK

                    char *fp = "/home/mubashir/banking/db/passbook/";
                    char send_filename[50];
                    char recv_filename[50];
                    // senders passbok
                    strcpy(send_filename, fp);
                    strcat(send_filename, record.username);
                    // recivers passbook
                    strcpy(recv_filename, fp);
                    strcat(recv_filename, recv_record.username);

                    // update passbook of sender
                    strcpy(s_info.sender_username, record.username);
                    strcpy(s_info.receiver_username, recv_record.username);
                    snprintf(s_info.amount, sizeof(s_info.amount), "%c%d", '-', transfer_info.amount);
                    s_info.balance = record.balance;
                    int send_fd = open(send_filename, O_APPEND | O_WRONLY);
                    write(send_fd, &s_info, sizeof(struct passbook_entry));
                    close(send_fd);

                    // update passbook of reciever
                    strcpy(r_info.sender_username, recv_record.username);
                    strcpy(r_info.receiver_username, record.username);
                    snprintf(r_info.amount, sizeof(r_info.amount), "%c%d", '+', transfer_info.amount);
                    r_info.balance = recv_record.balance;
                    int recv_fd = open(recv_filename, O_APPEND | O_WRONLY);
                    write(recv_fd, &r_info, sizeof(struct passbook_entry));
                    close(recv_fd);

                    strcpy(wbuffer, "Transfer Successfull:");
                    send(client_socket, wbuffer, BUFFER_SIZE, 0);
                }
                close(fd);
                release_lock(fd, sizeof(struct customer) * ptr->index, sizeof(struct customer));
                release_lock(fd, sizeof(struct customer) * ptr_to_recv->index, sizeof(struct customer));
            }
        }
        else if (selected_service.service_type == 5)
        {
            struct loan msg;
            recv(client_socket, &msg, sizeof(struct loan), 0);
            msg.status = 0;
            char *fp = "/home/mubashir/banking/db/loan";
            int fd = open(fp, O_RDWR | O_APPEND);
            struct stat st;
            stat(fp, &st);
            if (st.st_size == 0)
            {
                msg.application_no = 0;
                write(fd, &msg, sizeof(struct loan));
                continue;
            }
            struct flock lock;
            memset(&lock, 0, sizeof(lock));
            lock.l_type = F_WRLCK;
            lock.l_whence = SEEK_END;
            lock.l_start = -sizeof(struct loan);
            lock.l_len = sizeof(struct loan);
            fcntl(fd, F_SETLKW, &lock);

            struct loan temp;
            lseek(fd, -sizeof(struct loan), SEEK_END);
            read(fd, &temp, sizeof(struct loan));
            msg.application_no = temp.application_no + 1;

            ssize_t bytes_written = write(fd, &msg, sizeof(struct loan));
            if (bytes_written == -1)
            {
                strcpy(wbuffer, "Can't Submit your application");
                send(client_socket, wbuffer, BUFFER_SIZE, 0);
            }
            else
            {
                strcpy(wbuffer, "Your application Submitted Successfully");
                send(client_socket, wbuffer, BUFFER_SIZE, 0);
            }
            lock.l_type = F_UNLCK;
            fcntl(fd, F_SETLK, &lock);
            close(fd);
        }
        else if (selected_service.service_type == 6)
        {
            int fd = open("/home/mubashir/banking/db/customer", O_RDWR);
            struct changePassword new_cred;
            recv(client_socket, &new_cred, sizeof(struct changePassword), 0);
            acquire_write_lock(fd, sizeof(struct customer) * ptr->index, sizeof(struct customer));
            lseek(fd, sizeof(record) * ptr->index, SEEK_SET);
            read(fd, &record, sizeof(struct customer));
            strcpy(record.password, new_cred.new_password);
            lseek(fd, sizeof(record) * ptr->index, SEEK_SET);
            write(fd, &record, sizeof(struct customer));
            release_lock(fd, sizeof(struct customer) * ptr->index, sizeof(struct customer));
            close(fd);
            strcpy(wbuffer, "Password changed Successfully");
            send(client_socket, wbuffer, BUFFER_SIZE, 0);
        }
        else if (selected_service.service_type == 7)
        {
            struct feedback msg;
            recv(client_socket, &msg, sizeof(struct feedback), 0);
            strcpy(msg.username, ptr->username);
            int fd = open("/home/mubashir/banking/db/feedback", O_APPEND | O_WRONLY);
            write(fd, &msg, sizeof(struct feedback));
            strcpy(wbuffer, "Feedback Submitted Successfully");
            send(client_socket, NULL, 0, 0);
        }
        else if (selected_service.service_type == 8)
        {
            int fd = open("/home/mubashir/banking/db/customer", O_RDWR);
            lseek(fd, sizeof(record) * ptr->index, SEEK_SET);
            read(fd, &record, sizeof(struct customer));
            close(fd);
            struct passbook_entry pe;
            int fd2 = open(record.filename, O_RDONLY);
            while (read(fd2, &pe, sizeof(struct passbook_entry)) > 0)
            {
                send(client_socket, &pe, sizeof(struct passbook_entry), 0);
            }
            struct passbook_entry end_msg = {0}; // Zeroed structure
            send(client_socket, &end_msg, sizeof(struct passbook_entry), 0);
        }

        else if (selected_service.service_type == 9)
        {
            int fd = open("/home/mubashir/banking/db/customer", O_RDWR);
            acquire_write_lock(fd, sizeof(struct customer) * ptr->index, sizeof(struct customer));
            lseek(fd, sizeof(record) * ptr->index, SEEK_SET);
            read(fd, &record, sizeof(struct customer));
            record.logged_in = 0;
            lseek(fd, sizeof(record) * ptr->index, SEEK_SET);
            write(fd, &record, sizeof(struct customer));
            release_lock(fd, sizeof(struct customer) * ptr->index, sizeof(struct customer));
            close(fd);
            strcpy(wbuffer, "Logging Out");
            send(client_socket, wbuffer, strlen(wbuffer), 0); // Send logout message to client
            close(client_socket);
            printf("Connection closed\n");
            pthread_exit(NULL);
        }
        else if (selected_service.service_type == 10)
        {
            int fd = open("/home/mubashir/banking/db/customer", O_RDWR);
            acquire_write_lock(fd, sizeof(struct customer) * ptr->index, sizeof(struct customer));
            lseek(fd, sizeof(record) * ptr->index, SEEK_SET);
            read(fd, &record, sizeof(struct customer));
            record.logged_in = 0;
            lseek(fd, sizeof(record) * ptr->index, SEEK_SET);
            write(fd, &record, sizeof(struct customer));
            release_lock(fd, sizeof(struct customer) * ptr->index, sizeof(struct customer));
            close(fd);
            strcpy(wbuffer, "Exiting");
            send(client_socket, wbuffer, BUFFER_SIZE, 0);
            close(client_socket);
            pthread_exit(NULL);
        }
    }
}

void Customer_login(const int client_socket, struct login_message *msg)
{
    printf("cust_client\n");
    fflush(stdout);
    // Log in client
    struct user_map *ptr;
    struct customer record;
    ptr = find_user(msg->username, &cust);
    if (ptr == NULL)
    {
        char *str = "User does not exist";
        send(client_socket, str, strlen(str), 0);
        close(client_socket);
        pthread_exit(NULL);
    }
    else
    {
        int fd = open("/home/mubashir/banking/db/customer", O_RDWR);
        lseek(fd, sizeof(record) * ptr->index, SEEK_SET);
        read(fd, &record, sizeof(struct customer));
        if (strcmp(record.password, msg->password) == 0 && record.logged_in == 0 && record.active == 1)
        {
            char *str = "Log in Successfull";
            record.logged_in = 1;
            acquire_write_lock(fd, sizeof(struct customer) * ptr->index, sizeof(struct customer));
            lseek(fd, sizeof(record) * ptr->index, SEEK_SET);
            write(fd, &record, sizeof(struct customer));
            release_lock(fd, sizeof(struct customer) * ptr->index, sizeof(struct customer));
            close(fd);
            send(client_socket, str, strlen(str), 0);
            sleep(1);
            Customer_functionalities(client_socket, ptr);
        }
        else
        {
            char *str = "Wrong Credentials";
            send(client_socket, str, strlen(str), 0);
            close(client_socket);
            close(fd);
            pthread_exit(NULL);
        }
    }
}
