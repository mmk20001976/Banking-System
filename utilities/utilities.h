#ifndef UTILITIES_H
#define UTILITIES_H
struct login_message
{
    char user_type;
    char username[30];
    char password[30];
};
struct customer
{
    int id;
    char username[25];
    char password[20];
    int balance;
    int logged_in;
    int active;
    char filename[50];
    int account_no;
    char name[30];
    int age;
};
struct employee
{
    int id;
    char username[25];
    char password[20];
    char name[30];
    char filename[50];
    int logged_in;
};
struct manager
{
    int id;
    char username[25];
    char password[20];
    int logged_in;
};
struct admin
{
    int id;
    char username[25];
    char password[20];
    int logged_in;
};
struct service
{
    int service_type;
};

struct amount
{
    int value;
};
struct transaction
{
    char receiver_username[25];
    int amount;
};
struct passbook_entry
{
    char sender_username[25];
    char receiver_username[25];
    char amount[10];
    int balance;
};
struct changePassword
{
    char new_password[20];
};
struct feedback
{
    char username[25];
    char message[100];
    char response[100];
};
struct modify
{
    char username[25];
    char feild[20];
    char value[30];
};
struct Account_Status{
    int status;
    char username[25];
};
struct loan{
    int application_no;
    char username[25];
    char name[30];
    char contact[15];
    int loan_amount;
    int repayment_period; // in months
    int income;
    int employment_status;
    int credit_score;
    int status;
};
struct LoanAssign
{
    int application_no;
    char username[25];
};
#endif