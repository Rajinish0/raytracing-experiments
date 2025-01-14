#include <iostream>

class A{
public:
	virtual void f(){ 
		std::cout << "A's calling" << std::endl;
	}
};

class B : public A {
public:
	void f() override {
		std::cout << "B's calling" << std::endl;
	}
};

int main(){
	A a;
	B b;

	a.f(); b.f();
	
	a = b;
	a.f();


	A *ap = new A();
	B *bp = new B();
	
	std::cout << "Starting mind fuckery" << std::endl;
	bp->A::f();
	std::cout << "Ending mind fuckery" << std::endl;

	ap->f(); bp->f();
	delete ap;
	ap = bp;
	ap->f();
}
