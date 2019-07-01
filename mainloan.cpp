#include "mainloan.hpp"

void main_loan::add_borrower(name acc_name, uint64_t b_id, string location,
                    uint64_t b_phone, asset loan_individual,
                    asset b_balance, checksum256 group_id, uint64_t credit_score)
{
  print("Adding borrower", acc_name);
  require_auth( _self ); //authorization of the contract account

    check(loan_individual.amount >= 0, "loan must be positive");
    check(b_balance.amount >= 0, "balance must be positive");
    check(credit_score>=300 && credit_score<=900, "credit score is always postive and ranges from 300-900");


    borr_table.emplace(get_self(), [&](auto& b) {
        b.acc_name = acc_name;
        b.b_id = b_id;
        b.location = location;
        b.b_phone = b_phone;
        b.loan_individual = loan_individual;
        b.b_balance = b_balance;
        b.group_id = group_id;
        b.credit_score = credit_score;

  /*
  borr_table.emplace(get_self(), [&](auto& b){
    b.acc_name = acc_name;
    b.b_id = b_id;
    b.location = location;
    b.*/
  });


}
///namespace eosio
EOSIO_DISPATCH(main_loan, (add_borrower))
