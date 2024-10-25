#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include "/home/mubashir/banking/utilities/utilities.h"
#define PORT 8080
#define BUFFER_SIZE 1024

void Customer_req(const int sock, struct login_message *msg)
{
    printf("Customer_req\n");
    while (1)
    {
        char rbuffer[BUFFER_SIZE];
        char wbuffer[BUFFER_SIZE];

        // print menu
        ssize_t bytes_read = recv(sock, wbuffer, BUFFER_SIZE, 0);
        wbuffer[bytes_read] = '\0';
        printf("%s\n", wbuffer);
        //    ----------------------------------------------------------

        struct service selected_service;
        printf("Select From The above Services:\n");
        scanf("%d", &selected_service.service_type);
        send(sock, &selected_service, sizeof(struct service), 0);
        if (selected_service.service_type == 1)
        {
            ssize_t bytes_read = recv(sock, rbuffer, BUFFER_SIZE, 0);
            rbuffer[bytes_read] = '\0';
            printf("Current Balance: %s\n", rbuffer);
        }
        else if (selected_service.service_type == 2)
        {
            struct amount deposit_amount;

            printf("Enter The amount you want to deposit: ");
            scanf("%d", &deposit_amount.value);
            send(sock, &deposit_amount, sizeof(struct amount), 0);
            ssize_t bytes_read = recv(sock, rbuffer, BUFFER_SIZE, 0);
            rbuffer[bytes_read] = '\0';
            printf("%s\n", rbuffer);
        }
        else if (selected_service.service_type == 3)
        {
            struct amount widthdraw_amount;

            printf("Enter The amount you want to withdraw: ");
            scanf("%d", &widthdraw_amount.value);
            send(sock, &widthdraw_amount, sizeof(struct amount), 0);
            ssize_t bytes_read = recv(sock, rbuffer, BUFFER_SIZE, 0);
            rbuffer[bytes_read] = '\0';
            printf("%s\n", rbuffer);
        }
        else if (selected_service.service_type == 4)
        {
            struct transaction msg;
            printf("Enter the username of the user you want to send money to; ");
            scanf("%s", msg.receiver_username);
            printf("\nEnter The ammount you want to transfer: ");
            scanf("%d", &msg.amount);
            send(sock, &msg, sizeof(struct transaction), 0);
            sleep(1);
            ssize_t bytes_read = recv(sock, rbuffer, BUFFER_SIZE, 0);
            rbuffer[bytes_read] = '\0';
            printf("%s\n", rbuffer);
        }
        else if (selected_service.service_type == 5)
        {
            struct loan msg;
            printf("Enter name:\n");
            while (getchar() != '\n')
                ;

            if (fgets(msg.name, 30, stdin) != NULL)
            {
                if (msg.name[strlen(msg.name) - 1] == '\n')
                {
                    msg.name[strlen(msg.name) - 1] = '\0';
                }
            }
            printf("Enter Contact info\n");
            scanf("%s", msg.contact);

            printf("Enter loan amount: ");
            scanf("%d", &msg.loan_amount);

            printf("Enter repayment period (in months): ");
            scanf("%d", &msg.repayment_period);

            printf("Enter monthly income: ");
            scanf("%d", &msg.income);

            printf("Enter employment status (1 for employed, 0 for unemployed): ");
            scanf("%d", &msg.employment_status);

            printf("Enter credit score: ");
            scanf("%d", &msg.credit_score);

            send(sock, &msg, sizeof(struct loan), 0);
            ssize_t bytes_read = recv(sock, rbuffer, BUFFER_SIZE, 0);
            rbuffer[bytes_read] = '\0';
            printf("%s\n", rbuffer);
        }
        else if (selected_service.service_type == 6)
        {
            struct changePassword msg;
            printf("Enter New Password: ");
            scanf("%s", msg.new_password);
            send(sock, &msg, sizeof(struct changePassword), 0);
            ssize_t bytes_read = recv(sock, rbuffer, BUFFER_SIZE, 0);
            rbuffer[bytes_read] = '\0';
            printf("%s\n", rbuffer);
        }
        else if (selected_service.service_type == 7)
        {
            struct feedback msg;
            printf("Enter your feedback message (up to 2047 characters):\n");
            while (getchar() != '\n')
                ;

            if (fgets(msg.message, 100, stdin) != NULL)
            {
                if (msg.message[strlen(msg.message) - 1] == '\n')
                {
                    msg.message[strlen(msg.message) - 1] = '\0';
                }
            }
            send(sock, &msg, sizeof(struct feedback), 0);
            ssize_t bytes_read = recv(sock, rbuffer, BUFFER_SIZE, 0);
            rbuffer[bytes_read] = '\0';
            printf("%s\n", rbuffer);
        }
        else if (selected_service.service_type == 8)
        {
            struct passbook_entry msg;
            while (recv(sock, &msg, sizeof(struct passbook_entry), 0) > 0)
            {
                if (msg.balance == 0 && msg.amount[0] == '\0' && msg.sender_username[0] == '\0' && msg.receiver_username[0] == '\0')
                    break;
                printf("Your username: %s\n", msg.sender_username);
                printf("Other username: %s\n", msg.receiver_username);
                printf("Amount: %s\n", msg.amount);
                printf("Remaining Balance: %d\n", msg.balance);
            }
            printf("END OF FILE\n");
        }
        else if (selected_service.service_type == 9)
        {
            ssize_t bytes_read = recv(sock, rbuffer, BUFFER_SIZE, 0);
            rbuffer[bytes_read] = '\0';
            printf("%s\n", rbuffer);
            close(sock);
            sleep(1);
            execl("/home/mubashir/banking/client/a.out", "a.out", NULL);
            exit(EXIT_SUCCESS);
        }
        else if (selected_service.service_type == 10)
        {
            ssize_t bytes_read = recv(sock, rbuffer, BUFFER_SIZE, 0);
            rbuffer[bytes_read] = '\0';
            printf("%s\n", rbuffer);
            close(sock);
            exit(EXIT_SUCCESS);
        }
    }
}
void Employee_req(const int sock, struct login_message *msg)
{
    printf("Employee_req\n");
    while (1)
    {

        char rbuffer[BUFFER_SIZE];
        char wbuffer[BUFFER_SIZE];

        // print menu
        ssize_t bytes_read = recv(sock, wbuffer, BUFFER_SIZE, 0);
        wbuffer[bytes_read] = '\0';
        printf("%s\n", wbuffer);
        //    ----------------------------------------------------------
        struct service selected_service;
        printf("Select From The above Services:\n");
        scanf("%d", &selected_service.service_type);
        send(sock, &selected_service, sizeof(struct service), 0);
        if (selected_service.service_type == 1)
        {
            struct customer new_customer;

            printf("Enter username: ");
            scanf("%s", new_customer.username);

            printf("Enter password: ");
            scanf("%s", new_customer.password);

            printf("Enter initial balance: ");
            scanf("%d", &new_customer.balance);

            new_customer.logged_in = 0;
            new_customer.active = 1;

            printf("Enter name (with spaces): ");
            getchar();

            fgets(new_customer.name, sizeof(new_customer.name), stdin);
            new_customer.name[strcspn(new_customer.name, "\n")] = '\0';

            printf("Enter age: ");
            scanf("%d", &new_customer.age);

            send(sock, &new_customer, sizeof(struct customer), 0);
            ssize_t bytes_read = recv(sock, rbuffer, BUFFER_SIZE, 0);
            rbuffer[bytes_read] = '\0';
            printf("%s\n", rbuffer);
        }
        else if (selected_service.service_type == 2)
        {
            struct modify msg;
            printf("Enter the username:\n");
            scanf("%s", msg.username);
            printf("Enter Feild you want to modify:\n");
            scanf("%s", msg.feild);
            printf("Enter New Value:\n");
            scanf("%s", msg.value);
            send(sock, &msg, sizeof(struct modify), 0);
            ssize_t bytes_read = recv(sock, rbuffer, BUFFER_SIZE, 0);
            rbuffer[bytes_read] = '\0';
        }
        else if (selected_service.service_type == 3)
        {
        }
        else if (selected_service.service_type == 4)
        {
            struct loan msg;
            struct loan zeroed_loan = {0};
            while (recv(sock, &msg, sizeof(struct loan), 0) > 0)
            {
                if (memcmp(&msg, &zeroed_loan, sizeof(struct loan)) == 0)
                    break;

                printf("Loan Application Details:\n");
                printf("Username: %s\n", msg.username);
                printf("Name: %s\n", msg.name);
                printf("Contact: %s\n", msg.contact);
                printf("Loan Amount: $%d\n", msg.loan_amount);
                printf("Repayment Period: %d months\n", msg.repayment_period);
                printf("Income: $%d\n", msg.income);
                printf("Employment Status: %s\n", msg.employment_status ? "Employed" : "Unemployed");
                printf("Credit Score: %d\n", msg.credit_score);
            }
        }
        else if (selected_service.service_type == 5)
        {
            printf("Enter Username:\n");
            scanf("%s", wbuffer);
            send(sock, wbuffer, BUFFER_SIZE, 0);
            ssize_t bytes_read = recv(sock, rbuffer, BUFFER_SIZE, 0);
            rbuffer[bytes_read] = '\0';
            printf("%s\n", rbuffer);
            if (strcmp(rbuffer, "User does not exist") != 0)
            {
                struct passbook_entry msg;
                while (recv(sock, &msg, sizeof(struct passbook_entry), 0) > 0)
                {
                    if (msg.balance == 0 && msg.amount[0] == '\0' && msg.sender_username[0] == '\0' && msg.receiver_username[0] == '\0')
                        break;
                    printf("Your username: %s\n", msg.sender_username);
                    printf("Other username: %s\n", msg.receiver_username);
                    printf("Amount: %s\n", msg.amount);
                    printf("Remaining Balance: %d\n", msg.balance);
                }
                printf("END OF FILE\n");
            }
        }
        else if (selected_service.service_type == 6)
        {
            struct changePassword msg;
            printf("Enter New Password: ");
            scanf("%s", msg.new_password);
            send(sock, &msg, sizeof(struct changePassword), 0);
            ssize_t bytes_read = recv(sock, rbuffer, BUFFER_SIZE, 0);
            rbuffer[bytes_read] = '\0';
            printf("%s\n", rbuffer);
        }
        else if (selected_service.service_type == 7)
        {
            ssize_t bytes_read = recv(sock, rbuffer, BUFFER_SIZE, 0);
            rbuffer[bytes_read] = '\0';
            printf("%s\n", rbuffer);
            close(sock);
            sleep(1);
            execl("/home/mubashir/banking/client/a.out", "a.out", NULL);
            perror("execl failed");
            exit(EXIT_SUCCESS);
        }
        else if (selected_service.service_type == 8)
        {
            ssize_t bytes_read = recv(sock, rbuffer, BUFFER_SIZE, 0);
            rbuffer[bytes_read] = '\0';
            printf("%s\n", rbuffer);
            close(sock);
            exit(EXIT_SUCCESS);
        }
    }
}
void Manager_req(const int sock, struct login_message *msg)
{
    while (1)
    {
        char rbuffer[BUFFER_SIZE];
        char wbuffer[BUFFER_SIZE];

        // print menu
        ssize_t bytes_read = recv(sock, wbuffer, BUFFER_SIZE, 0);
        wbuffer[bytes_read] = '\0';
        printf("%s\n", wbuffer);
        //    ----------------------------------------------------------
        struct service selected_service;
        printf("Select From The above Services:\n");
        scanf("%d", &selected_service.service_type);
        send(sock, &selected_service, sizeof(struct service), 0);
        if (selected_service.service_type == 1)
        {
            struct Account_Status msg;
            printf("Enter username of Customer:\n");
            scanf("%s", msg.username);
            printf("Enter 1 to Activate and 0 to Deactivate");
            scanf("%d", &msg.status);
            send(sock, &msg, sizeof(struct Account_Status), 0);
            ssize_t bytes_read = recv(sock, rbuffer, BUFFER_SIZE, 0);
            rbuffer[bytes_read] = '\0';
            printf("%s\n", rbuffer);
        }
        else if (selected_service.service_type == 2)
        {
            struct LoanAssign msg;
            printf("Enter username of Employee\n");
            scanf("%s", msg.username);
            printf("Enter Apllication number:\n");
            scanf("%d", &msg.application_no);
            send(sock, &msg, sizeof(struct LoanAssign), 0);
            ssize_t bytes_read = recv(sock, rbuffer, BUFFER_SIZE, 0);
            rbuffer[bytes_read] = '\0';
            printf("%s\n", rbuffer);
        }
        else if (selected_service.service_type == 3)
        {
            struct feedback msg;
            struct feedback zeroed_feedback = {0};
            while (recv(sock, &msg, sizeof(struct feedback), 0) > 0)
            {
                if (memcmp(&msg, &zeroed_feedback, sizeof(struct feedback)) == 0)
                    break;
                printf("Customer Username: %s", msg.username);
                printf("Message: %s", msg.message);
                printf("Enter Reply:\n");
                while (getchar() != '\n')
                    ;
                if (fgets(msg.response, 50, stdin) != NULL)
                {
                    if (msg.response[strlen(msg.response) - 1] == '\n')
                    {
                        msg.response[strlen(msg.response) - 1] = '\0';
                    }
                }
                send(sock, &msg, sizeof(struct feedback), 0);
                printf("Enter 1 if you wish to discontinue and 0 to continue");
                scanf("%s", wbuffer);
                send(sock, wbuffer, BUFFER_SIZE, 0);
            }
        }
        else if (selected_service.service_type == 4)
        {
            struct changePassword msg;
            printf("Enter New Password: ");
            scanf("%s", msg.new_password);
            send(sock, &msg, sizeof(struct changePassword), 0);
            ssize_t bytes_read = recv(sock, rbuffer, BUFFER_SIZE, 0);
            rbuffer[bytes_read] = '\0';
            printf("%s\n", rbuffer);
        }
        else if (selected_service.service_type == 5)
        {
            size_t bytes_read = recv(sock, rbuffer, BUFFER_SIZE, 0);
            rbuffer[bytes_read] = '\0';
            printf("%s\n", rbuffer);
            close(sock); // Close socket before execl
            sleep(1);    // Optional: Give server time to process the logout
            execl("/home/mubashir/banking/client/a.out", "a.out", NULL);
            perror("execl failed"); // This will only execute if execl fails
            exit(EXIT_SUCCESS);
        }
        else if (selected_service.service_type == 6)
        {
            ssize_t bytes_read = recv(sock, rbuffer, BUFFER_SIZE, 0);
            rbuffer[bytes_read] = '\0';
            printf("%s\n", rbuffer);
            close(sock);
            exit(EXIT_SUCCESS);
        }
    }
}
void Admin_req(const int sock, struct login_message *msg)
{
    while (1)
    {
        char rbuffer[BUFFER_SIZE];
        char wbuffer[BUFFER_SIZE];

        // print menu
        ssize_t bytes_read = recv(sock, wbuffer, BUFFER_SIZE, 0);
        wbuffer[bytes_read] = '\0';
        printf("%s\n", wbuffer);
        //    ----------------------------------------------------------
        struct service selected_service;
        printf("Select From The above Services:\n");
        scanf("%d", &selected_service.service_type);
        send(sock, &selected_service, sizeof(struct service), 0);
        if (selected_service.service_type == 1)
        {
            struct employee new_employee;

            printf("Enter username: ");
            scanf("%s", new_employee.username);

            printf("Enter password: ");
            scanf("%s", new_employee.password);

            new_employee.logged_in = 0;
            printf("Enter name (with spaces): ");
            getchar();

            fgets(new_employee.name, sizeof(new_employee.name), stdin);
            new_employee.name[strcspn(new_employee.name, "\n")] = '\0';

            send(sock, &new_employee, sizeof(struct employee), 0);
            ssize_t bytes_read = recv(sock, rbuffer, BUFFER_SIZE, 0);
            rbuffer[bytes_read] = '\0';
            printf("%s\n", rbuffer);
        }
        else if (selected_service.service_type == 2)
        {
            struct modify msg;
            printf("Enter the username:\n");
            scanf("%s", msg.username);
            printf("Enter Feild you want to modify:\n");
            scanf("%s", msg.feild);
            printf("Enter New Value:\n");
            scanf("%s", msg.value);
            send(sock, &msg, sizeof(struct modify), 0);
            ssize_t bytes_read = recv(sock, rbuffer, BUFFER_SIZE, 0);
            rbuffer[bytes_read] = '\0';
        }
        else if (selected_service.service_type == 3)
        {
            struct changePassword msg;
            printf("Enter New Password: ");
            scanf("%s", msg.new_password);
            send(sock, &msg, sizeof(struct changePassword), 0);
            ssize_t bytes_read = recv(sock, rbuffer, BUFFER_SIZE, 0);
            rbuffer[bytes_read] = '\0';
            printf("%s\n", rbuffer);
        }
        else if (selected_service.service_type == 4)
        {
            ssize_t bytes_read = recv(sock, rbuffer, BUFFER_SIZE, 0);
            rbuffer[bytes_read] = '\0';
            printf("%s\n", rbuffer);
            close(sock);
            sleep(1);
            execl("/home/mubashir/banking/client/a.out", "a.out", NULL);
            perror("execl failed");
            exit(EXIT_SUCCESS);
        }
        else if (selected_service.service_type == 5)
        {
            ssize_t bytes_read = recv(sock, rbuffer, BUFFER_SIZE, 0);
            rbuffer[bytes_read] = '\0';
            printf("%s\n", rbuffer);
            close(sock);
            exit(EXIT_SUCCESS);
        }
    }
}

int main()
{
    struct login_message msg;
    char buffer[BUFFER_SIZE] = {0};

    printf("Welcome To Indus Banks\n");
    printf("Sign In:\n");
    printf("1) Customer\n");
    printf("2) Employee\n");
    printf("3) Manager\n");
    printf("4) Admin\n");
    int choice;
    printf("Enter your choice: ");
    scanf("%d", &choice);
    switch (choice)
    {
    case 1:
        msg.user_type = 'C';
        break;
    case 2:
        msg.user_type = 'E';
        break;
    case 3:
        msg.user_type = 'M';
        break;
    case 4:
        msg.user_type = 'A';
        break;
    }
    printf("\nUsername: ");
    scanf("%s", msg.username);
    printf("\nPassword: ");
    scanf("%s", msg.password);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("Socket creation error");
        return -1;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        perror("Invalid address/Address not supported");
        close(sock);
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Connection Failed");
        close(sock);
        return -1;
    }

    if (send(sock, &msg, sizeof(struct login_message), 0) < 0)
    {
        perror("Send failed");
        close(sock);
        return -1;
    }
    printf("Message sent to server\n");

    ssize_t bytes_read = recv(sock, buffer, BUFFER_SIZE, 0);
    if (bytes_read < 0)
    {
        perror("Receive failed");
        close(sock);
        return -1;
    }

    if (bytes_read > 0)
    {
        buffer[bytes_read] = '\0';
    }

    if (strcmp(buffer, "Wrong Credentials") == 0 || strcmp(buffer, "User does not exist") == 0)
    {
        printf("%s\n", buffer);
        close(sock);
        sleep(2);
        execl("/home/mubashir/banking/client/a.out", "a.out", NULL);
        exit(EXIT_SUCCESS);

        // If execl fails, handle that case here
    }
    else
    {
        printf("%s\n", buffer);
        if (msg.user_type == 'C')
            Customer_req(sock, &msg);
        else if (msg.user_type == 'E')
            Employee_req(sock, &msg);
        else if (msg.user_type == 'M')
            Manager_req(sock, &msg);
        else
            Admin_req(sock, &msg);
    }

    close(sock); // Ensure the socket is closed at the end
    return 0;
}