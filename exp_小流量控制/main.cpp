#include"exp.h"

int main()
{
    Exp exp("exp.conf");
    string posid = "0_5";
    string flag = "Flag_enable_order_by_bid";
    cout << exp.getBoolValue(posid,flag)<<endl;
    return 0;
}
