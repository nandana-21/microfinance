#include "mainloan.hpp"
#include <string>
using namespace eosio;
using namespace std;

void ACTION add_borrower(name acc_name, uint64_t b_id, vector<string> location,
                    uint64_t b_phone, uint64_t group_type, asset loan_individual,
                    asset b_balance, uint64_t group_id, uint64_t credit_score)
{
  


}
///namespace eosio
EOSIO_DISPATCH(main_loan, (lender_to_contract))
