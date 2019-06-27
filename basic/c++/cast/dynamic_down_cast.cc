#include <iostream>
using namespace std;

class A{
public:
    virtual void func() const { cout<<"Class A"<<endl; }
private:
    int m_a;
};

class B: public A{
public:
    virtual void func() const { cout<<"Class B"<<endl; }
private:
    int m_b;
};

class C: public B{
public:
    virtual void func() const { cout<<"Class C"<<endl; }
private:
    int m_c;
};

class D: public C{
public:
    virtual void func() const { cout<<"Class D"<<endl; }
private:
    int m_d;
};

int main(){
    A *pa = new A();
    B *pb;
    C *pc;
   
    // ��1
    pb = dynamic_cast<B*>(pa);  // ������
    if(pb == NULL){
        cout<<"Downcasting failed: A* to B*"<<endl;
    }else{
        cout<<"Downcasting successfully: A* to B*"<<endl;
        pb -> func();
    }
    pc = dynamic_cast<C*>(pa);  // ������
    if(pc == NULL){
        cout<<"Downcasting failed: A* to C*"<<endl;
    }else{
        cout<<"Downcasting successfully: A* to C*"<<endl;
        pc -> func();
    }
   
    cout<<"-------------------------"<<endl;
   
    // ��2
    pa = new D();  // ��������
    pb = dynamic_cast<B*>(pa);  // ������
    if(pb == NULL){
        cout<<"Downcasting failed: A* to B*"<<endl;
    }else{
        cout<<"Downcasting successfully: A* to B*"<<endl;
        pb -> func();
    }
    pc = dynamic_cast<C*>(pa);  //����ת�ͳɹ�
    if(pc == NULL){
        cout<<"Downcasting failed: A* to C*"<<endl;
    }else{
        cout<<"Downcasting successfully: A* to C*"<<endl;
        pc -> func();
    }
   
    return 0;
}
