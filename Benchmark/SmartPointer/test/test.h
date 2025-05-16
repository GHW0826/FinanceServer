#pragma once
#include <memory>
#include <iostream>
#include <iomanip>
#include <string>
#include "StdSharedPtr.h"
#include "TestObject.h"


void StdUniquePtrTest()
{
    std::cout << "===== unique_ptr Test =====\n";
    // 생성
    std::unique_ptr<TestObject> up1(new TestObject("unique1"));
    // std::unique_ptr<TestObject> up2 = up1; // compile error (copy not allowed)

    // move로 소유권 이전
    std::unique_ptr<TestObject> up2 = std::move(up1);

	std::shared_ptr<TestObject> sp1(new TestObject("shared1"));

    // up1 is now null
    if (!up1) {
        std::cout << "up1 is null after move\n";
    }
    // up2 owns "unique1" object
}

void StdSharedPtrTest() {

    int margin1 = 17;
    int margin2 = 25;
	int margin3 = 15;
	int margin4 = 5;

    std::cout << "===== shared_ptr Test =====\n";

    // [1] shared_ptr 생성 (참조 카운트 1)
    std::shared_ptr<TestObject> p1(new TestObject("p1"));
    cout << left << setw(margin1) << "1) [new] p1"
        << left << setw(margin2) << "[p1]"
        << left << setw(margin3) << "p1 use count : "
        << left << setw(margin4) << p1.use_count() << "\n\n";

    // [2] shared_ptr 대입 (참조 카운트 증가)
    std::shared_ptr<TestObject> p2 = p1;
    cout << left << setw(margin1) << "2) p2 = p1"
        << left << setw(margin2) << "[p1, p2]"
		<< left << setw(margin3) << "p1 use count : " << left << setw(margin4) << p1.use_count()
		<< left << setw(margin3) << "p2 use count : " << left << setw(margin4) << p2.use_count() << "\n\n";

    // [3] 복사 (참조 카운트 증가)
    std::shared_ptr<TestObject> p3(p1);
    cout << left << setw(margin1) << "3) p3(p1)"
        << left << setw(margin2) << "[p1, p2, p3]"
        << left << setw(margin3) << "p1 use count : " << left << setw(margin4) << p1.use_count()
        << left << setw(margin3) << "p2 use count : " << left << setw(margin4) << p2.use_count()
        << left << setw(margin3) << "p3 use count : " << left << setw(margin4) << p3.use_count() << "\n\n";

    // [4] 이동 (참조 카운트 유지, p1은 비어있음)
    std::shared_ptr<TestObject> p4 = std::move(p1);
    cout << left << setw(margin1) << "4) p4 = move(p1)"
        << left << setw(margin2) << "[p2, p3, p4]"
        << left << setw(margin3) << "p1 use count : " << left << setw(margin4) << (p1 ? p1.use_count() : 0)
        << left << setw(margin3) << "p2 use count : " << left << setw(margin4) << p2.use_count()
        << left << setw(margin3) << "p3 use count : " << left << setw(margin4) << p3.use_count()
        << left << setw(margin3) << "p4 use count : " << left << setw(margin4) << p4.use_count() << "\n\n";

    // [5] 새로운 shared_ptr 생성 (다른 객체, 참조 카운트 1)
    std::shared_ptr<TestObject> p5(new TestObject("p5"));
    cout << left << setw(margin1) << "5) [new] p5"
        << left << setw(margin2) << "[p2, p3, p4] [p5]"
        << left << setw(margin3) << "p2 use count : " << left << setw(margin4) << p2.use_count()
        << left << setw(margin3) << "p3 use count : " << left << setw(margin4) << p3.use_count()
        << left << setw(margin3) << "p4 use count : " << left << setw(margin4) << p4.use_count()
        << left << setw(margin3) << "p5 use count : " << left << setw(margin4) << p5.use_count() << "\n\n";

    // [6] shared_ptr 대입 (p3가 객체2를 가리키도록 변경)
    p3 = p5;
    cout << left << setw(margin1) << "6) p3 = p5"
        << left << setw(margin2) << "[p2, p4] [p3, p5]"
        << left << setw(margin3) << "p2 use count : " << left << setw(margin4) << p2.use_count()
        << left << setw(margin3) << "p3 use count : " << left << setw(margin4) << p3.use_count()
        << left << setw(margin3) << "p4 use count : " << left << setw(margin4) << p4.use_count()
        << left << setw(margin3) << "p5 use count : " << left << setw(margin4) << p5.use_count() << "\n\n";


    // [7] 새로운 shared_ptr 생성 및 기존 포인터에 이동 대입
    std::shared_ptr<TestObject> p6(new TestObject("p6"));
    cout << left << setw(margin1) << "7) [new] p6"
        << left << setw(margin2) << "[p2, p4] [p3, p5] [p6]"
        << left << setw(margin3) << "p6 use count : "
        << left << setw(margin4) << p6.use_count() << "\n\n";


    p6 = std::move(p5);
    cout << left << setw(margin1) << "8) p6 = move(p5)"
        << left << setw(margin2) << "[p2, p4] [p3, p6]"
        << left << setw(margin3) << "p2 use count : " << left << setw(margin4) << p2.use_count()
        << left << setw(margin3) << "p3 use count : " << left << setw(margin4) << p3.use_count()
        << left << setw(margin3) << "p4 use count : " << left << setw(margin4) << p4.use_count()
        << left << setw(margin3) << "p5 use count : " << left << setw(margin4) << (p5 ? p5.use_count() : 0)
        << left << setw(margin3) << "p6 use count : " << left << setw(margin4) << p6.use_count() << "\n\n";

    // [8] 일부 포인터 해제
    p4.reset();  // 객체1에 대한 한 참조 제거
    cout << left << setw(margin1) << "8) p4.reset()"
        << left << setw(margin2) << "[p2] [p3, p6]"
        << left << setw(margin3) << "p2 use count : " << left << setw(margin4) << p2.use_count()
        << left << setw(margin3) << "p3 use count : " << left << setw(margin4) << p3.use_count()
        << left << setw(margin3) << "p4 use count : " << left << setw(margin4) << p4.use_count() 
        << left << setw(margin3) << "p6 use count : " << left << setw(margin4) << p6.use_count() << "\n\n"; // (객체1 count는 2->1로 감소)


    p3.reset();  // 객체2에 대한 한 참조 제거
    cout << left << setw(margin1) << "9) p3.reset()"
        << left << setw(margin2) << "[p2] [p6]"
        << left << setw(margin3) << "p2 use count : " << left << setw(margin4) << p2.use_count()
        << left << setw(margin3) << "p3 use count : " << left << setw(margin4) << p3.use_count()
        << left << setw(margin3) << "p4 use count : " << left << setw(margin4) << p4.use_count()
        << left << setw(margin3) << "p6 use count : " << left << setw(margin4) << p6.use_count() << "\n\n"; // (객체2 count는 2->1로 감소)

    std::shared_ptr<TestObject> r1;


    p2.reset();  // 객체2에 대한 한 참조 제거
    cout << left << setw(margin1) << "10) p2.reset()"
        << left << setw(margin2) << "[p6]"
        << left << setw(margin3) << "p2 use count : " << left << setw(margin4) << p2.use_count()
        << left << setw(margin3) << "p3 use count : " << left << setw(margin4) << p3.use_count()
        << left << setw(margin3) << "p4 use count : " << left << setw(margin4) << p4.use_count()
        << left << setw(margin3) << "p6 use count : " << left << setw(margin4) << p6.use_count() << "\n\n";

    p6.reset();  // 객체2에 대한 한 참조 제거
    cout << left << setw(margin1) << "11) p6.reset()"
        << left << setw(margin2) << "[]"
        << left << setw(margin3) << "p2 use count : " << left << setw(margin4) << p2.use_count()
        << left << setw(margin3) << "p3 use count : " << left << setw(margin4) << p3.use_count()
        << left << setw(margin3) << "p4 use count : " << left << setw(margin4) << p4.use_count()
        << left << setw(margin3) << "p6 use count : " << left << setw(margin4) << p6.use_count() << "\n\n";
}



// weak_ptr lock()으로 객체가 살아 있는지 확인하는 함수
void CheckSharedObject(const std::weak_ptr<TestObject>& wptr) {
    // lock()을 통해 shared_ptr를 얻음. 만약 객체가 이미 해제되었다면 빈 포인터를 반환.
    auto sptr = wptr.lock();
    if (sptr) {
        std::cout << "Object " << sptr->getName() << "' is alive, use_count=" << sptr.use_count() << "\n";
    } else {
        std::cout << "Object is expired (no longer alive).\n";
    }
}

struct CustomDeleter {
    void operator()(TestObject* ptr) {
        std::cout << "[CustomDeleter] Deleting object: " << ptr->getName() << std::endl;
        delete ptr;
    }
};

void StdWeakPtrTest() {

    int margin1 = 17;
    int margin2 = 25;
    int margin3 = 15;
    int margin4 = 5;

    std::cout << "===== weak_ptr Test =====\n";

    // [1] shared_ptr 생성 (참조 카운트 1)
    std::shared_ptr<TestObject> s1(new TestObject("s1"));
    cout << left << setw(margin1) << "1) [new] s1"
        << left << setw(margin2) << "[s1]"
        << left << setw(margin3) << "s1 use count : "
        << left << setw(margin4) << s1.use_count() << "\n\n";

    // 2) weak_ptr로 약하게 참조  weak_ptr는 use_count에는 변동이 없음
    std::weak_ptr<TestObject> w1 = s1;
    cout << left << setw(margin1) << "2) w1 = s1"
        << left << setw(margin2) << "[s1 (w1)]"
        << left << setw(margin3) << "s1 use count : "
        << left << setw(margin4) << s1.use_count() << "\n\n";


    // 3) checkObjectStatus 함수로 상태 확인
    CheckSharedObject(w1);
    cout << left << setw(margin1) << "3) w1.lock"
        << left << setw(margin2) << "[s1 (w1)]"
        << left << setw(margin3) << "s1 use count : "
        << left << setw(margin4) << s1.use_count() << "\n\n";

    {
        // 4) 또 다른 shared_ptr를 만들면 참조 카운트 2
        auto s2 = s1;
        cout << left << setw(margin1) << "4) s2 = s1"
            << left << setw(margin2) << "[s1, s2, (w1)]"
            << left << setw(margin3) << "s1 use count : " << left << setw(margin4) << s1.use_count()
            << left << setw(margin3) << "s2 use count : " << left << setw(margin4) << s2.use_count() << "\n\n";

        // 5) 또 다른 weak_ptr도 생성
        std::weak_ptr<TestObject> w2 = s2;
        cout << left << setw(margin1) << "5) w2 = s2"
            << left << setw(margin2) << "[s1, s2, (w1, w2)]"
            << left << setw(margin3) << "s1 use count : " << left << setw(margin4) << s1.use_count()
            << left << setw(margin3) << "s2 use count : " << left << setw(margin4) << s2.use_count() << "\n\n";

        CheckSharedObject(w2);
    }
    // 6) 이 스코프를 벗어날 때 s2, w2가 소멸 (참조 카운트 감소)
    cout << left << setw(margin1) << "6) scope out"
        << left << setw(margin2) << "[s1 (w1)]"
        << left << setw(margin3) << "s1 use count : " << left << setw(margin4) << s1.use_count() << "\n\n";

    // 7) shared_ptr s1 해제 -> 객체 메모리 해제
    s1.reset();
    cout << left << setw(margin1) << "7) s1.reset()"
        << left << setw(margin2) << "[(w1)]"
        << left << setw(margin3) << "s1 use count : " << left << setw(margin4) << s1.use_count() << "\n\n";
    // 이제 Obj1은 더 이상 어떤 shared_ptr도 가리키지 않음 -> 객체 해제

    // 8) weak_ptr가 expired 상태가 되었는지 확인
    CheckSharedObject(w1);


    // ControlBlock Addres Capture
    std::shared_ptr<TestObject> spB1(new TestObject("NewObj"), CustomDeleter{});
    std::weak_ptr<TestObject> w3 = spB1;
    {
        {
            auto spB2 = spB1;
            std::cout << "spB1 use_count=" << spB1.use_count() << std::endl; // 2
        }
        // spB2 소멸 -> use_count=1
        std::cout << "spB1 use_count after spB2 gone = " << spB1.use_count() << std::endl; // 1
    }
    spB1.reset();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void CustomSharedPtrTest() {

    int margin1 = 17;
    int margin2 = 25;
    int margin3 = 15;
    int margin4 = 5;

    std::cout << "===== shared_ptr Test =====\n";

    // [1] shared_ptr 생성 (참조 카운트 1)
    StdSharedPtr<TestObject> p1(new TestObject("p1"));
    cout << left << setw(margin1) << "1) [new] p1"
        << left << setw(margin2) << "[p1]"
        << left << setw(margin3) << "p1 use count : "
        << left << setw(margin4) << p1.use_count() << "\n\n";

    // [2] shared_ptr 대입 (참조 카운트 증가)
    StdSharedPtr<TestObject> p2 = p1;
    cout << left << setw(margin1) << "2) p2 = p1"
        << left << setw(margin2) << "[p1, p2]"
        << left << setw(margin3) << "p1 use count : " << left << setw(margin4) << p1.use_count()
        << left << setw(margin3) << "p2 use count : " << left << setw(margin4) << p2.use_count() << "\n\n";

    // [3] 복사 (참조 카운트 증가)
    StdSharedPtr<TestObject> p3(p1);
    cout << left << setw(margin1) << "3) p3(p1)"
        << left << setw(margin2) << "[p1, p2, p3]"
        << left << setw(margin3) << "p1 use count : " << left << setw(margin4) << p1.use_count()
        << left << setw(margin3) << "p2 use count : " << left << setw(margin4) << p2.use_count()
        << left << setw(margin3) << "p3 use count : " << left << setw(margin4) << p3.use_count() << "\n\n";

    // [4] 이동 (참조 카운트 유지, p1은 비어있음)
    StdSharedPtr<TestObject> p4 = std::move(p1);
    cout << left << setw(margin1) << "4) p4 = move(p1)"
        << left << setw(margin2) << "[p2, p3, p4]"
        << left << setw(margin3) << "p1 use count : " << left << setw(margin4) << (p1 ? p1.use_count() : 0)
        << left << setw(margin3) << "p2 use count : " << left << setw(margin4) << p2.use_count()
        << left << setw(margin3) << "p3 use count : " << left << setw(margin4) << p3.use_count()
        << left << setw(margin3) << "p4 use count : " << left << setw(margin4) << p4.use_count() << "\n\n";

    // [5] 새로운 shared_ptr 생성 (다른 객체, 참조 카운트 1)
    StdSharedPtr<TestObject> p5(new TestObject("p5"));
    cout << left << setw(margin1) << "5) [new] p5"
        << left << setw(margin2) << "[p2, p3, p4] [p5]"
        << left << setw(margin3) << "p2 use count : " << left << setw(margin4) << p2.use_count()
        << left << setw(margin3) << "p3 use count : " << left << setw(margin4) << p3.use_count()
        << left << setw(margin3) << "p4 use count : " << left << setw(margin4) << p4.use_count()
        << left << setw(margin3) << "p5 use count : " << left << setw(margin4) << p5.use_count() << "\n\n";

    // [6] shared_ptr 대입 (p3가 객체2를 가리키도록 변경)
    p3 = p5;
    cout << left << setw(margin1) << "6) p3 = p5"
        << left << setw(margin2) << "[p2, p4] [p3, p5]"
        << left << setw(margin3) << "p2 use count : " << left << setw(margin4) << p2.use_count()
        << left << setw(margin3) << "p3 use count : " << left << setw(margin4) << p3.use_count()
        << left << setw(margin3) << "p4 use count : " << left << setw(margin4) << p4.use_count()
        << left << setw(margin3) << "p5 use count : " << left << setw(margin4) << p5.use_count() << "\n\n";


    // [7] 새로운 shared_ptr 생성 및 기존 포인터에 이동 대입
    StdSharedPtr<TestObject> p6(new TestObject("p6"));
    cout << left << setw(margin1) << "7) [new] p6"
        << left << setw(margin2) << "[p2, p4] [p3, p5] [p6]"
        << left << setw(margin3) << "p6 use count : "
        << left << setw(margin4) << p6.use_count() << "\n\n";


    p6 = std::move(p5);
    cout << left << setw(margin1) << "8) p6 = move(p5)"
        << left << setw(margin2) << "[p2, p4] [p3, p6]"
        << left << setw(margin3) << "p2 use count : " << left << setw(margin4) << p2.use_count()
        << left << setw(margin3) << "p3 use count : " << left << setw(margin4) << p3.use_count()
        << left << setw(margin3) << "p4 use count : " << left << setw(margin4) << p4.use_count()
        << left << setw(margin3) << "p5 use count : " << left << setw(margin4) << (p5 ? p5.use_count() : 0)
        << left << setw(margin3) << "p6 use count : " << left << setw(margin4) << p6.use_count() << "\n\n";

    // [8] 일부 포인터 해제
    p4.reset();  // 객체1에 대한 한 참조 제거
    cout << left << setw(margin1) << "8) p4.reset()"
        << left << setw(margin2) << "[p2] [p3, p6]"
        << left << setw(margin3) << "p2 use count : " << left << setw(margin4) << p2.use_count()
        << left << setw(margin3) << "p3 use count : " << left << setw(margin4) << p3.use_count()
        << left << setw(margin3) << "p4 use count : " << left << setw(margin4) << p4.use_count()
        << left << setw(margin3) << "p6 use count : " << left << setw(margin4) << p6.use_count() << "\n\n"; // (객체1 count는 2->1로 감소)


    p3.reset();  // 객체2에 대한 한 참조 제거
    cout << left << setw(margin1) << "9) p3.reset()"
        << left << setw(margin2) << "[p2] [p6]"
        << left << setw(margin3) << "p2 use count : " << left << setw(margin4) << p2.use_count()
        << left << setw(margin3) << "p3 use count : " << left << setw(margin4) << p3.use_count()
        << left << setw(margin3) << "p4 use count : " << left << setw(margin4) << p4.use_count()
        << left << setw(margin3) << "p6 use count : " << left << setw(margin4) << p6.use_count() << "\n\n"; // (객체2 count는 2->1로 감소)



    p2.reset();  // 객체2에 대한 한 참조 제거
    cout << left << setw(margin1) << "10) p2.reset()"
        << left << setw(margin2) << "[p6]"
        << left << setw(margin3) << "p2 use count : " << left << setw(margin4) << p2.use_count()
        << left << setw(margin3) << "p3 use count : " << left << setw(margin4) << p3.use_count()
        << left << setw(margin3) << "p4 use count : " << left << setw(margin4) << p4.use_count()
        << left << setw(margin3) << "p6 use count : " << left << setw(margin4) << p6.use_count() << "\n\n";

    p6.reset();  // 객체2에 대한 한 참조 제거
    cout << left << setw(margin1) << "11) p6.reset()"
        << left << setw(margin2) << "[]"
        << left << setw(margin3) << "p2 use count : " << left << setw(margin4) << p2.use_count()
        << left << setw(margin3) << "p3 use count : " << left << setw(margin4) << p3.use_count()
        << left << setw(margin3) << "p4 use count : " << left << setw(margin4) << p4.use_count()
        << left << setw(margin3) << "p6 use count : " << left << setw(margin4) << p6.use_count() << "\n\n";
}