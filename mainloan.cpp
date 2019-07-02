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
EOSIO_DISPATCH(mainloan, (addborrower)(getborrower))
