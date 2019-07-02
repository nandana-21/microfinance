#include "/Users/macbookpro/Documents/contracts/microfinance/mainloan.hpp"

void mainloan::addborrower(name acc_name, uint64_t b_id, string location,
                    uint64_t b_phone, asset loan_individual,
                    asset b_balance, uint64_t group_id, uint64_t credit_score)
{
  print("Adding borrower: ", acc_name);
  require_auth( _self ); //authorization of the contract account

    eosio::check(loan_individual.amount >= 0, "loan must be positive");
    eosio::check(b_balance.amount >= 0, "balance must be positive");
    eosio::check(credit_score>=300 && credit_score<=900, "credit score is always postive and ranges from 300-900");


    borr_table.emplace(get_self(), [&](auto& b) {
        b.acc_name = acc_name;
        b.b_id = b_id;
        b.location = location;
        b.b_phone = b_phone;
        b.loan_individual = loan_individual;
        b.b_balance = b_balance;
        b.group_id = group_id;
        b.credit_score = credit_score;
  });
}

void mainloan::getborrower(name acc_name){

void mainloan::addgroupinfo(uint64_t group_id, asset total_loan, vector <string> member_names)
{
    print("Adding group info", acc_name);
    require_auth( _self );

      check(total_loan.amount >= 0, "loan must be positive");

      group_table.emplace(get_self(), [&](auto& g) {
          g.group_id = group_id;
          g.total_loan = total_loan;
          g.member_names = member_names;  //check once
});

void mainloan::addunderwriter(name acc_name, uint64_t acc_id,
                                asset balance, uint64_t value_score)
{
    print("Adding underwriter", acc_name);
    require_auth( _self );

      check(balance.amount >= 0, "balance must be positive");
      check(value_score>=300 && value_score<=900, "credit score is always postive and ranges from 300-900");


      uwr_table.emplace(get_self(), [&](auto& l) {
          u.acc_name = acc_name;
          u.acc_id = acc_id;
          u.balance = balance;
          u.value_score = value_score;
});

void mainloan::addrelayer(name acc_name, uint64_t acc_id, asset balance)
{
    print("Adding relayer", acc_name);
    require_auth( _self );

      check(balance.amount >= 0, "balance must be positive");

      uwr_table.emplace(get_self(), [&](auto& r) {
          r.acc_name = acc_name;
          r.acc_id = acc_id;
          r.balance = balance;
});


void mainloan::addlender(name acc_name, uint64_t acc_id, asset balance)
{
  print("Adding lender", acc_name);
  require_auth( _self );

    check(balance.amount >= 0, "balance must be positive");

    lender_table.emplace(get_self(), [&](auto& l) {
        l.acc_name = acc_name;
        l.acc_id = acc_id;
        l.balance = balance;
});


void mainloan::addloaninfo(name acc_name, asset lending_amount, uint64_t lent_group_id,
                          uint64_t interest_rate, uint64_t payment_time, uint64_t status)
{
  print("Adding loan info", acc_name);
  require_auth( _self );

    check(lending_amount.amount >= 0, "amount must be positive");
    check(interest_rate.amount >= 0, "interest rate must be positive");

    loan_table.emplace(get_self(), [&](auto& a) {
        a.acc_name = acc_name;
        a.lending_amount = lending_amount;
        a.lent_group_id = lent_group_id;
        a.interest_rate = interest_rate;
        a.payment_time = payment_time;
        a.status = status;
});

void mainloan::getborrower(name acc_name){

  auto borrower = borr_table.get(acc_name.value);
  eosio::check(borrower.acc_name==acc_name, "Borrower doesn't exist.\n");

  print("Borrower Details: ", borrower.acc_name);
  print("ID: ", borrower.b_id);
  print("Location: ", borrower.location);
  print("Phone Number: ", borrower.b_phone);
  print("Loan Individual: ", borrower.loan_individual);
  print("Balance: ", borrower.b_balance);
  print("Group ID: ", borrower.group_id);
  print("Credit Score: ", borrower.credit_score);

}















///namespace eosio
EOSIO_DISPATCH(mainloan, (addborrower)(addgroupinfo)(addunderwriter)(addrelayer)(addlender)(addloaninfo))
