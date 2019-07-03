#include "/Users/macbookpro/Documents/contracts/mainloan/mainloan.hpp"

void mainloan::addborrower(name acc_name, uint64_t b_id, string location,
                    uint64_t b_phone, asset loan_individual,
                    asset b_balance, uint64_t group_id, uint64_t credit_score)
{
  print("Adding borrower: ", acc_name);
  require_auth( _self ); //authorization of the contract account

    eosio::check(loan_individual.amount >= 0, "loan must be positive");
    eosio::check(b_balance.amount >= 0, "balance must be positive");

    borr_table.emplace(get_self(), [&](auto& b) {
        b.acc_name = acc_name;
        b.b_id = b_id;
        b.location = location;
        b.b_phone = b_phone;
        b.loan_individual = loan_individual;
        b.b_balance = b_balance;
  });
}

void mainloan::adduwr(name acc_name, uint64_t acc_id, asset balance)
{
    print("Adding underwriter", acc_name);
    require_auth( _self );

      eosio::check(balance.amount >= 0, "balance must be positive");
      //eosio::check(value_score>=300 && value_score<=900, "credit score is always postive and ranges from 300-900");

      uwr_table.emplace(get_self(), [&](auto& u) {
          u.acc_name = acc_name;
          u.acc_id = acc_id;
          u.balance = balance;
      });
}

void mainloan::getborrower(name acc_name){

  auto borrower = borr_table.get(acc_name.value);
  eosio::check(borrower.acc_name==acc_name, "Borrower doesn't exist.");

  eosio::print("Borrower Details: ", borrower.acc_name);
  eosio::print(" ID: ", borrower.b_id);
  eosio::print("Location: ", borrower.location);
  eosio::print("Phone Number: ", borrower.b_phone);
  eosio::print("Loan Individual: ", borrower.loan_individual);
  eosio::print("Balance: ", borrower.b_balance);
  eosio::print("Credit Score: ", borrower.credit_score);
}

///namespace eosio
EOSIO_DISPATCH(mainloan, (addborrower)(getborrower))
