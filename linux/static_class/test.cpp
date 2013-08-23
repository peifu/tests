#include <iostream>

using namespace std;

class Test {
private:
    int var_a;
    int var_b;

public:
    void set_a(int a) {
        var_a =  a;
    }
    void set_b(int b) {
        var_b =  b;
    }

    int get_a(void) {
        return var_a;
    }
    int get_b(void) {
        return var_b;
    }
};


class Test2 {
private:
    static int var_a;
    static int var_b;

public:
    static void set_a(int a) {
        var_a = a;
    }
    static void set_b(int b) {
        var_b = b;
    }
    static int get_a(void) {
        return var_a;
    }
    static int get_b(void) {
        return var_b;
    }
};

int Test2::var_a = 0;
int Test2::var_b = 0;

int main()
{
    Test *t = new Test();
    Test2 t2;


    t->set_a(1);
    t->set_b(2);

    cout<<"Test.a: "<<t->get_a()<<endl;
    cout<<"Test.b: "<<t->get_b()<<endl;
    
    Test2::set_a(3);
    Test2::set_b(4);

    cout<<"Test2.a: "<<Test2::get_a()<<endl;
    cout<<"Test2.b: "<<Test2::get_b()<<endl;
    
    return 0;
}
