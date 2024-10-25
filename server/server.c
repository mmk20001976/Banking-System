#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <fcntl.h>
#include "/home/mubashir/banking/utilities/utilities.h"
#include "/home/mubashir/banking/server/map.h"
#include "locks.h"
#include "customer.h"
#include <sys/stat.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_ENTRIES 15
#define SHM_SIZE (sizeof(struct user_map) * MAX_ENTRIES) // Size for 4 maps

struct user_map *cust = NULL;
struct user_map *emp = NULL;
struct user_map *manager = NULL;
struct user_map *admin = NULL;


void Employee_functionalities(const int client_socket, struct user_map *ptr)
{
    printf("Emp Func\n");
    fflush(stdout);
    while (1)
    {
        char rbuffer[BUFFER_SIZE];
        char wbuffer[BUFFER_SIZE];
        char *menu = "1) Add New Customer\n"
                     "2) Modify Customer Details\n"
                     "3) Approve / Reject Loans\n"
                     "4) View Assigned Loan Applications\n"
                     "5) View Customer Transactions\n"
                     "6) Change Password\n"
                     "7) Logout\n"
                     "8) Exit\n";

        send(client_socket, menu, strlen(menu), 0);
        sleep(2);
        struct service selected_service;
        recv(client_socket, &selected_service, sizeof(struct service), 0);
        struct employee record;

        if (selected_service.service_type == 1)
        {
            struct customer new;
            recv(client_socket, &new, sizeof(struct customer), 0);
            struct user_map *ptr_to_data;
            ptr_to_data = find_user(new.username,&cust);
            if (ptr_to_data == NULL)
            {
                strcpy(wbuffer, "Username already exists");
                send(client_socket, wbuffer, BUFFER_SIZE, 0);
            }
            else
            {
                char *fp = "/home/mubashir/banking/db/passbook/";
                strcpy(new.filename, fp);
                strcat(new.filename, new.username);
                int fd = open("/home/mubashir/banking/db/customer", O_RDWR | O_APPEND);
                int fd2 = open(new.filename, O_CREAT, 0666);
                close(fd2);
                struct flock lock;
                memset(&lock, 0, sizeof(lock));
                lock.l_type = F_WRLCK;
                lock.l_whence = SEEK_END;
                lock.l_start = -sizeof(struct customer);
                lock.l_len = sizeof(struct customer);
                fcntl(fd, F_SETLKW, &lock);
                struct customer temp;
                lseek(fd, -sizeof(struct customer), SEEK_END);
                read(fd, &temp, sizeof(struct customer));
                new.account_no = temp.account_no + 1;
                new.id = temp.id + 1;
                lseek(fd, 0, SEEK_END);
                write(fd, &new, sizeof(struct customer));
                add_user(new.username, new.id, &cust);
                lock.l_type = F_UNLCK;
                fcntl(fd, F_SETLK, &lock);
                strcpy(wbuffer, "New Customer Added Successfully");
                send(client_socket, wbuffer, BUFFER_SIZE, 0);
            }
        }
        else if (selected_service.service_type == 2)
        {
            struct modify msg;
            recv(client_socket, &msg, sizeof(struct modify), 0);
            struct user_map *ptr_to_cr;
            ptr_to_cr = find_user(msg.username, &cust);
            if (ptr_to_cr == NULL)
            {
                strcpy(wbuffer, "User does not exist");
                send(client_socket, wbuffer, BUFFER_SIZE, 0);
            }
            else
            {
                struct customer c;
                // struct customer c;
                int fd = open("/home/mubashir/banking/db/customer", O_RDWR);
                acquire_write_lock(fd, sizeof(struct customer) * ptr_to_cr->index, sizeof(struct customer));
                lseek(fd, sizeof(struct customer) * ptr_to_cr->index, SEEK_SET);
                read(fd, &c, sizeof(struct customer));
                if (strcmp(msg.feild, "name") == 0)
                {
                    strcpy(c.name, msg.value);
                }
                else if (strcmp(msg.feild, "age") == 0)
                {
                    c.age = atoi(msg.value);
                }
                lseek(fd, sizeof(struct customer) * ptr_to_cr->index, SEEK_SET);
                write(fd, &c, sizeof(struct customer));
                release_lock(fd, sizeof(struct customer) * ptr_to_cr->index, sizeof(struct customer));
                strcpy(wbuffer, "Details Updated Successfully");
                send(client_socket, wbuffer, BUFFER_SIZE, 0);
            }
        }
        else if (selected_service.service_type == 3)
        {
        }
        else if (selected_service.service_type == 4)
        {
            char *fp = "/home/mubashir/banking/db/assignedLoan/";
            char filename[50];
            strcpy(filename, fp);
            strcat(filename, ptr->username);
            int fd = open(filename, O_RDWR);
            struct loan l;
            while (read(fd, &l, sizeof(struct loan)) > 0)
            {
                send(client_socket, &l, sizeof(struct loan), 0);
            }
            struct loan end_msg = {0}; // Zeroed structure
            send(client_socket, &end_msg, sizeof(struct loan), 0);
        }
        else if (selected_service.service_type == 5)
        {
            ssize_t bytes_read = recv(client_socket, rbuffer, BUFFER_SIZE, 0);
            rbuffer[bytes_read] = '\0';
            struct user_map *ptr_to_cr;
            ptr_to_cr = find_user(rbuffer, &cust);
            if (ptr_to_cr == NULL)
            {
                strcpy(wbuffer, "User does not exist");
                send(client_socket, wbuffer, BUFFER_SIZE, 0);
            }
            else
            {
                strcpy(wbuffer, "User Exists");
                send(client_socket, wbuffer, BUFFER_SIZE, 0);
                struct customer c;
                int fd = open("/home/mubashir/banking/db/customer", O_RDWR);
                acquire_read_lock(fd, sizeof(struct customer) * ptr_to_cr->index, sizeof(struct customer));
                lseek(fd, sizeof(c) * ptr_to_cr->index, SEEK_SET);
                read(fd, &c, sizeof(struct customer));
                release_lock(fd, sizeof(struct customer) * ptr_to_cr->index, sizeof(struct customer));
                close(fd);
                struct passbook_entry pe;
                int fd2 = open(c.filename, O_RDONLY);
                while (read(fd2, &pe, sizeof(struct passbook_entry)) > 0)
                {
                    send(client_socket, &pe, sizeof(struct passbook_entry), 0);
                }
                struct passbook_entry end_msg = {0}; // Zeroed structure
                send(client_socket, &end_msg, sizeof(struct passbook_entry), 0);
            }
        }
        else if (selected_service.service_type == 6)
        {
            int fd = open("/home/mubashir/banking/db/employee", O_RDWR);
            struct changePassword new_cred;
            recv(client_socket, &new_cred, sizeof(struct changePassword), 0);
            acquire_write_lock(fd, sizeof(struct employee) * ptr->index, sizeof(struct employee));
            lseek(fd, sizeof(record) * ptr->index, SEEK_SET);
            read(fd, &record, sizeof(struct employee));
            strcpy(record.password, new_cred.new_password);
            lseek(fd, sizeof(record) * ptr->index, SEEK_SET);
            write(fd, &record, sizeof(struct employee));
            release_lock(fd, sizeof(struct employee) * ptr->index, sizeof(struct employee));
            close(fd);
            strcpy(wbuffer, "Password changed Successfully");
            send(client_socket, wbuffer, BUFFER_SIZE, 0);
        }
        else if (selected_service.service_type == 7)
        {
            int fd = open("/home/mubashir/banking/db/employee", O_RDWR);
            acquire_write_lock(fd, sizeof(struct employee) * ptr->index, sizeof(struct employee));
            lseek(fd, sizeof(record) * ptr->index, SEEK_SET);
            read(fd, &record, sizeof(struct employee));
            record.logged_in = 0;
            lseek(fd, sizeof(record) * ptr->index, SEEK_SET);
            write(fd, &record, sizeof(struct employee));
            release_lock(fd, sizeof(struct employee) * ptr->index, sizeof(struct employee));
            close(fd);
            strcpy(wbuffer, "Logging Out");
            send(client_socket, wbuffer, BUFFER_SIZE, 0);
            close(client_socket);
            printf("connection closed\n");
            pthread_exit(NULL);
        }
        else if (selected_service.service_type == 8)
        {
            int fd = open("/home/mubashir/banking/db/employee", O_RDWR);
            acquire_write_lock(fd, sizeof(struct employee) * ptr->index, sizeof(struct employee));
            lseek(fd, sizeof(record) * ptr->index, SEEK_SET);
            read(fd, &record, sizeof(struct employee));
            record.logged_in = 0;
            lseek(fd, sizeof(record) * ptr->index, SEEK_SET);
            write(fd, &record, sizeof(struct employee));
            release_lock(fd, sizeof(struct employee) * ptr->index, sizeof(struct employee));
            close(fd);
            strcpy(wbuffer, "Exiting");
            send(client_socket, wbuffer, BUFFER_SIZE, 0);
            close(client_socket);
            printf("connection closed\n");
            pthread_exit(NULL);
        }
    }
}

void Manager_functionalities(const int client_socket, struct user_map *ptr)
{
    printf("Emp Func\n");
    fflush(stdout);
    while (1)
    {
        char rbuffer[BUFFER_SIZE];
        char wbuffer[BUFFER_SIZE];
        char *menu = "1) Activate / Deactivate Customer Accounts\n"
                     "2) Assign Loan Application Processes to Employees\n"
                     "3) Review Customer Feedback\n"
                     "4) Change Password\n"
                     "5) Logout\n"
                     "6) Exit\n";

        send(client_socket, menu, strlen(menu), 0);
        sleep(2);
        struct service selected_service;
        recv(client_socket, &selected_service, sizeof(struct service), 0);
        struct manager record;
        if (selected_service.service_type == 1)
        {
            struct Account_Status msg;
            recv(client_socket, &msg, sizeof(struct Account_Status), 0);
            struct user_map *ptr_to_data = find_user(msg.username, &cust);
            if (ptr_to_data == NULL)
            {
                strcpy(wbuffer, "User does not exist");
                send(client_socket, wbuffer, BUFFER_SIZE, 0);
            }
            else
            {
                struct customer c;
                int fd = open("/home/mubashir/banking/db/customer", O_RDWR);
                acquire_write_lock(fd, sizeof(struct customer) * ptr_to_data->index, sizeof(struct customer));
                lseek(fd, sizeof(struct customer) * ptr_to_data->index, SEEK_SET);
                read(fd, &c, sizeof(struct customer));
                if (msg.status == 1)
                {
                    c.active = 1;
                }
                else
                    c.active = 0;
                lseek(fd, sizeof(struct customer) * ptr->index, SEEK_SET);
                write(fd, &c, sizeof(struct customer));
                release_lock(fd, sizeof(struct customer) * ptr_to_data->index, sizeof(struct customer));
                if (msg.status == 1)
                {
                    strcpy(wbuffer, "Activated Successfully");
                    send(client_socket, wbuffer, BUFFER_SIZE, 0);
                }
                else
                {
                    strcpy(wbuffer, "Deactivated Successfully");
                    send(client_socket, wbuffer, BUFFER_SIZE, 0);
                }
            }
        }
        else if (selected_service.service_type == 2)
        {
            struct LoanAssign msg;
            recv(client_socket, &msg, sizeof(struct LoanAssign), 0);
            int fd = open("/home/mubashir/banking/db/loan", O_RDWR);
            acquire_write_lock(fd, msg.application_no * sizeof(struct LoanAssign), sizeof(struct LoanAssign));
            lseek(fd, msg.application_no * sizeof(struct LoanAssign), SEEK_SET);
            struct loan l;
            read(fd, &l, sizeof(struct loan));
            if (strcmp(l.username, msg.username) != 0)
            {
                strcpy(wbuffer, "Incorrect Info");
                send(client_socket, wbuffer, BUFFER_SIZE, 0);
                continue;
            }
            if (l.status == 1)
            {
                strcpy(wbuffer, "Already Assigned to someone");
                send(client_socket, wbuffer, BUFFER_SIZE, 0);
                continue;
            }
            l.status = 1;
            lseek(fd, msg.application_no * sizeof(struct LoanAssign), SEEK_SET);
            read(fd, &l, sizeof(struct loan));
            char *fp = "/home/mubashir/banking/db/assignedLoan/";
            char filename[50];
            strcpy(filename, fp);
            strcat(filename, msg.username);
            int fd2 = open(filename, O_WRONLY | O_APPEND | O_CREAT, 0666);
            write(fd2, &l, sizeof(struct LoanAssign));
            close(fd2);
            release_lock(fd, msg.application_no * sizeof(struct LoanAssign), sizeof(struct LoanAssign));
            close(fd);
            strcpy(wbuffer, "Application Assigned");
            send(client_socket, wbuffer, BUFFER_SIZE, 0);
        }
        else if (selected_service.service_type == 3)
        {
            int fd = open("/home/mubashir/banking/db/feedback", O_RDWR);
            struct feedback c;
            while (read(fd, &c, sizeof(struct feedback)) > 0)
            {
                send(client_socket, &c, sizeof(struct feedback), 0);
                recv(client_socket, &c, sizeof(struct feedback), 0);
                lseek(fd, -sizeof(struct feedback), SEEK_CUR);
                write(fd, &c, sizeof(struct feedback));
                ssize_t bytes_read = recv(client_socket, rbuffer, BUFFER_SIZE, 0);
                rbuffer[bytes_read] = '\0';
                if (strcmp(rbuffer, "1") == 0)
                    break;
            }
            struct feedback end_msg = {0}; // Zeroed structure
            send(client_socket, &end_msg, sizeof(struct passbook_entry), 0);
        }
        else if (selected_service.service_type == 4)
        {
            int fd = open("/home/mubashir/banking/db/manager", O_RDWR);
            struct changePassword new_cred;
            recv(client_socket, &new_cred, sizeof(struct changePassword), 0);
            acquire_write_lock(fd, sizeof(struct manager) * ptr->index, sizeof(struct manager));
            lseek(fd, sizeof(record) * ptr->index, SEEK_SET);
            read(fd, &record, sizeof(struct manager));
            strcpy(record.password, new_cred.new_password);
            lseek(fd, sizeof(record) * ptr->index, SEEK_SET);
            write(fd, &record, sizeof(struct manager));
            release_lock(fd, sizeof(struct manager) * ptr->index, sizeof(struct manager));
            close(fd);
            strcpy(wbuffer, "Password changed Successfully");
            send(client_socket, wbuffer, BUFFER_SIZE, 0);
        }
        else if (selected_service.service_type == 5)
        {
            int fd = open("/home/mubashir/banking/db/manager", O_RDWR);
            acquire_write_lock(fd, sizeof(struct manager) * ptr->index, sizeof(struct manager));
            lseek(fd, sizeof(record) * ptr->index, SEEK_SET);
            read(fd, &record, sizeof(struct manager));
            record.logged_in = 0;
            lseek(fd, sizeof(record) * ptr->index, SEEK_SET);
            write(fd, &record, sizeof(struct manager));
            release_lock(fd, sizeof(struct manager) * ptr->index, sizeof(struct manager));
            close(fd);
            strcpy(wbuffer, "Logging Out");
            send(client_socket, wbuffer, BUFFER_SIZE, 0);
            close(client_socket);
            printf("connection closed\n");
            pthread_exit(NULL);
        }
        else if (selected_service.service_type == 6)
        {
            int fd = open("/home/mubashir/banking/db/manager", O_RDWR);
            acquire_write_lock(fd, sizeof(struct manager) * ptr->index, sizeof(struct manager));
            lseek(fd, sizeof(record) * ptr->index, SEEK_SET);
            read(fd, &record, sizeof(struct manager));
            record.logged_in = 0;
            lseek(fd, sizeof(record) * ptr->index, SEEK_SET);
            write(fd, &record, sizeof(struct manager));
            release_lock(fd, sizeof(struct manager) * ptr->index, sizeof(struct manager));
            close(fd);
            strcpy(wbuffer, "Exiting");
            send(client_socket, wbuffer, BUFFER_SIZE, 0);
            close(client_socket);
            printf("connection closed\n");
            pthread_exit(NULL);
        }
    }
}

void Admin_functionalities(const int client_socket, struct user_map *ptr)
{
    printf("Admin Func\n");
    fflush(stdout);
    while (1)
    {
        char rbuffer[BUFFER_SIZE];
        char wbuffer[BUFFER_SIZE];
        char *menu = "1) Add New Bank Employee\n"
                     "2) Modify Customer / Employee Details\n"
                     "3) Change Password\n"
                     "4) Logout\n"
                     "5) Exit\n";

        send(client_socket, menu, strlen(menu), 0);
        sleep(2);
        struct service selected_service;
        recv(client_socket, &selected_service, sizeof(struct service), 0);
        struct admin record;
        if (selected_service.service_type == 1)
        {
            struct employee new;
            recv(client_socket, &new, sizeof(struct employee), 0);
            struct user_map *ptr_to_data;
            ptr_to_data = find_user(new.username,&emp);
            if (ptr_to_data == NULL)
            {
                strcpy(wbuffer, "Username already exists");
                send(client_socket, wbuffer, BUFFER_SIZE, 0);
            }
            else
            {
                char *fp = "/home/mubashir/banking/db/assignedLoan";
                strcpy(new.filename, fp);
                strcat(new.filename, new.username);
                int fd = open("/home/mubashir/banking/db/employee", O_RDWR | O_APPEND);
                int fd2 = open(new.filename, O_CREAT, 0666);
                close(fd2);
                struct flock lock;
                memset(&lock, 0, sizeof(lock));
                lock.l_type = F_WRLCK;
                lock.l_whence = SEEK_END;
                lock.l_start = -sizeof(struct employee);
                lock.l_len = sizeof(struct employee);
                fcntl(fd, F_SETLKW, &lock);
                struct employee temp;
                lseek(fd, -sizeof(struct employee), SEEK_END);
                read(fd, &temp, sizeof(struct employee));
                new.id = temp.id + 1;
                lseek(fd, 0, SEEK_END);
                write(fd, &new, sizeof(struct employee));
                add_user(new.username, new.id, &emp);
                lock.l_type = F_UNLCK;
                fcntl(fd, F_SETLK, &lock);
                strcpy(wbuffer, "New Employee Added Successfully");
                send(client_socket, wbuffer, BUFFER_SIZE, 0);
            }
        }
        else if (selected_service.service_type == 2)
        {
            struct modify msg;
            recv(client_socket, &msg, sizeof(struct modify), 0);
            struct user_map *ptr_to_cr;
            ptr_to_cr = find_user(msg.username, &cust);
            if (ptr_to_cr == NULL)
            {
                strcpy(wbuffer, "User does not exist");
                send(client_socket, wbuffer, BUFFER_SIZE, 0);
            }
            else
            {
                struct customer c;
                // struct customer c;
                int fd = open("/home/mubashir/banking/db/customer", O_RDWR);
                acquire_write_lock(fd, sizeof(struct customer) * ptr_to_cr->index, sizeof(struct customer));
                lseek(fd, sizeof(struct customer) * ptr_to_cr->index, SEEK_SET);
                read(fd, &c, sizeof(struct customer));
                if (strcmp(msg.feild, "name") == 0)
                {
                    strcpy(c.name, msg.value);
                }
                else if (strcmp(msg.feild, "age") == 0)
                {
                    c.age = atoi(msg.value);
                }
                lseek(fd, sizeof(struct customer) * ptr_to_cr->index, SEEK_SET);
                write(fd, &c, sizeof(struct customer));
                release_lock(fd, sizeof(struct customer) * ptr_to_cr->index, sizeof(struct customer));
                strcpy(wbuffer, "Details Updated Successfully");
                send(client_socket, wbuffer, BUFFER_SIZE, 0);
            }
        }
        else if (selected_service.service_type == 3)
        {
            int fd = open("/home/mubashir/banking/db/admin", O_RDWR);
            struct changePassword new_cred;
            recv(client_socket, &new_cred, sizeof(struct changePassword), 0);
            acquire_write_lock(fd, sizeof(struct manager) * ptr->index, sizeof(struct manager));
            lseek(fd, sizeof(record) * ptr->index, SEEK_SET);
            read(fd, &record, sizeof(struct manager));
            strcpy(record.password, new_cred.new_password);
            lseek(fd, sizeof(record) * ptr->index, SEEK_SET);
            write(fd, &record, sizeof(struct manager));
            release_lock(fd, sizeof(struct manager) * ptr->index, sizeof(struct manager));
            close(fd);
            strcpy(wbuffer, "Password changed Successfully");
            send(client_socket, wbuffer, BUFFER_SIZE, 0);
        }
        else if (selected_service.service_type == 4)
        {
            int fd = open("/home/mubashir/banking/db/admin", O_RDWR);
            acquire_write_lock(fd, sizeof(struct manager) * ptr->index, sizeof(struct manager));
            lseek(fd, sizeof(record) * ptr->index, SEEK_SET);
            read(fd, &record, sizeof(struct manager));
            record.logged_in = 0;
            lseek(fd, sizeof(record) * ptr->index, SEEK_SET);
            write(fd, &record, sizeof(struct manager));
            release_lock(fd, sizeof(struct manager) * ptr->index, sizeof(struct manager));
            strcpy(wbuffer, "Logging Out");
            send(client_socket, wbuffer, BUFFER_SIZE, 0);
            close(fd);
            close(client_socket);
            printf("connection closed\n");
            pthread_exit(NULL);
        }
        else if (selected_service.service_type == 5)
        {
            int fd = open("/home/mubashir/banking/db/admin", O_RDWR);
            acquire_write_lock(fd, sizeof(struct manager) * ptr->index, sizeof(struct manager));
            lseek(fd, sizeof(record) * ptr->index, SEEK_SET);
            read(fd, &record, sizeof(struct manager));
            record.logged_in = 0;
            lseek(fd, sizeof(record) * ptr->index, SEEK_SET);
            write(fd, &record, sizeof(struct manager));
            release_lock(fd, sizeof(struct manager) * ptr->index, sizeof(struct manager));
            strcpy(wbuffer, "Exiting");
            send(client_socket, wbuffer, BUFFER_SIZE, 0);
            close(fd);
            close(client_socket);
            printf("connection closed\n");
            pthread_exit(NULL);
        }
    }
}

void Employee_login(const int client_socket, struct login_message *msg)
{
    printf("emp_client\n");
    fflush(stdout);
    // Log in client
    struct user_map *ptr;
    struct employee record;
    ptr = find_user(msg->username, &emp);
    if (ptr == NULL)
    {
        char *str = "User does not exist";
        send(client_socket, str, strlen(str), 0);
        close(client_socket);
        pthread_exit(NULL);
    }
    else
    {
        int fd = open("/home/mubashir/banking/db/employee", O_RDWR);
        lseek(fd, sizeof(record) * ptr->index, SEEK_SET);
        read(fd, &record, sizeof(struct employee));
        if (strcmp(record.password, msg->password) == 0 && record.logged_in == 0)
        {
            char *str = "Log in Successfull";
            record.logged_in = 1;
            acquire_write_lock(fd, sizeof(struct employee) * ptr->index, sizeof(struct employee));
            lseek(fd, sizeof(record) * ptr->index, SEEK_SET);
            write(fd, &record, sizeof(struct employee));
            release_lock(fd, sizeof(struct employee) * ptr->index, sizeof(struct employee));
            close(fd);
            send(client_socket, str, strlen(str), 0);
            sleep(1);
            Employee_functionalities(client_socket, ptr);
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

void Manager_login(const int client_socket, struct login_message *msg)
{
    printf("manager_client\n");
    fflush(stdout);
    // Log in client
    struct user_map *ptr;
    struct manager record;
    ptr = find_user(msg->username, &manager);
    if (ptr == NULL)
    {
        char *str = "User does not exist";
        send(client_socket, str, strlen(str), 0);
        close(client_socket);
        pthread_exit(NULL);
    }
    else
    {
        int fd = open("/home/mubashir/banking/db/manager", O_RDWR);
        lseek(fd, sizeof(record) * ptr->index, SEEK_SET);
        read(fd, &record, sizeof(struct manager));
        if (strcmp(record.password, msg->password) == 0 && record.logged_in == 0)
        {
            char *str = "Log in Successfull";
            acquire_write_lock(fd, sizeof(struct manager) * ptr->index, sizeof(struct manager));
            record.logged_in = 1;
            lseek(fd, sizeof(record) * ptr->index, SEEK_SET);
            write(fd, &record, sizeof(struct manager));
            release_lock(fd, sizeof(struct manager) * ptr->index, sizeof(struct manager));
            close(fd);
            send(client_socket, str, strlen(str), 0);
            sleep(1);
            Manager_functionalities(client_socket, ptr);
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

void Admin_login(const int client_socket, struct login_message *msg)
{
    printf("admin_client\n");
    fflush(stdout);
    // Log in client
    struct user_map *ptr;
    struct manager record;
    ptr = find_user(msg->username, &admin);
    if (ptr == NULL)
    {
        char *str = "User does not exist";
        send(client_socket, str, strlen(str), 0);
        close(client_socket);
        pthread_exit(NULL);
    }
    else
    {
        int fd = open("/home/mubashir/banking/db/admin", O_RDWR);
        lseek(fd, sizeof(record) * ptr->index, SEEK_SET);
        read(fd, &record, sizeof(struct manager));
        if (strcmp(record.password, msg->password) == 0 && record.logged_in == 0)
        {
            char *str = "Log in Successfull";
            acquire_write_lock(fd, sizeof(struct manager) * ptr->index, sizeof(struct manager));
            record.logged_in = 1;
            lseek(fd, sizeof(record) * ptr->index, SEEK_SET);
            write(fd, &record, sizeof(struct manager));
            release_lock(fd, sizeof(struct manager) * ptr->index, sizeof(struct manager));
            close(fd);
            send(client_socket, str, strlen(str), 0);
            sleep(1);
            Admin_functionalities(client_socket, ptr);
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

void handle_client(const int client_socket)
{
    printf("Handling client...\n");

    struct login_message msg;
    ssize_t bytes_received = recv(client_socket, &msg, sizeof(struct login_message), 0);
    printf("client: %c\n", msg.user_type);

    if (bytes_received <= 0)
    {
        perror("Failed to receive login message");
        close(client_socket);
        return;
    }

    switch (msg.user_type)
    {
    case 'C':
        Customer_login(client_socket, &msg);
        break;
    case 'E':
        Employee_login(client_socket, &msg);
        break;
    case 'M':
        Manager_login(client_socket, &msg);
        break;
    case 'A':
        Admin_login(client_socket, &msg);
        break;
    default:
        printf("Unknown user type: %d\n", msg.user_type);
        break;
    }

    close(client_socket); // Ensure the socket is closed after handling
}

void *client_handler(void *socket_desc)
{
    int client_socket = *(int *)socket_desc;
    free(socket_desc); // Free the allocated memory
    handle_client(client_socket);
    return NULL;
}

int main()
{

    int cust_file = open("/home/mubashir/banking/db/customer", O_RDWR);
    struct customer c_record;
    while (read(cust_file, &c_record, sizeof(struct customer)) > 0)
    {
        printf("username: %s\n", c_record.username);
        printf("id: %d\n", c_record.id);
        add_user(c_record.username, c_record.id, &cust);
    }
    close(cust_file);

    // Employee Map
    int emp_file = open("/home/mubashir/banking/db/employee", O_RDWR);
    struct employee e_record;
    while (read(emp_file, &e_record, sizeof(struct employee)) > 0)
    {
        printf("username: %s\n", e_record.username);
        printf("id: %d\n", e_record.id);
        add_user(e_record.username, e_record.id, &emp);
    }
    close(emp_file);

    // Manager Map
    int manager_file = open("/home/mubashir/banking/db/manager", O_RDWR);
    struct manager m_record;
    while (read(manager_file, &m_record, sizeof(struct manager)) > 0)
    {
        printf("username: %s\n", m_record.username);
        printf("id: %d\n", m_record.id);
        add_user(m_record.username, m_record.id, &manager);
    }
    close(manager_file);

    // Admin Map
    int admin_file = open("/home/mubashir/banking/db/admin", O_RDWR);
    struct customer a_record;
    while (read(admin_file, &a_record, sizeof(struct customer)) > 0)
    {
        printf("username: %s\n", a_record.username);
        printf("id: %d\n", a_record.id);
        // int num = atoi(a_record.id);
        add_user(a_record.username, a_record.id, &cust);
    }
    close(admin_file);

    // Establishing a socket
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;         // IPv4
    address.sin_addr.s_addr = INADDR_ANY; // Any incoming address
    address.sin_port = htons(PORT);       // Convert port to network byte order

    // Bind the socket to the network address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d\n", PORT);

    while (1)
    {

        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        printf("Connection accepted from client\n");

        int *new_sock = malloc(1);
        *new_sock = new_socket;

        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, client_handler, (void *)new_sock) < 0)
        {
            perror("Could not create thread");
            free(new_sock);
        }

        pthread_detach(thread_id);
    }

    close(server_fd);
    return 0;
}
