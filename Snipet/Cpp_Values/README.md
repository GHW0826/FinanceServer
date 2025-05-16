# Values   

```
          Expression (Value Category)
              /                \
         glvalue               rvalue
          /    \              /      \
      lvalue   xvalue     prvalue    xvalue(?)

rvalue = xvalue + prvalue
glvalue = lvalue + xvalue
```

## lvalue (locator value)   
- C++03까지는 식이 메모리에 존재하는 객체를 지칭.
- C++11에서 lvalue와 xvalue를 합쳐 glvalue(generalized lvalue)라고 부름.   
- 메모리상의 이름이 있는 객체, 주소를 얻을 수 있고, 대입문 왼쪽에 쓸 수 있음 그 메모리 구역은 일정 기간(스코프) 유지.
- 예) 변수 이름(`x`), 참조(`ref`), 배열 원소(`arr[i]`), `*ptr`, 변수 `int hello` 등
- **“이름 있는 객체**, **참조 가능 객체**, **메모리 상의 위치**(“객체”)를 지칭.   


  - 함수 오버로드 시, lvalue는 `const std::string&` 매개변수로 전달되어 적절한 오버로드가 호출됩니다.


## rvalue (read value)   
- C++03까지는 **rvalue**: 임시값. 식의 결과가 “더 이상 사용하지 않을 일시적 값.”
- C++11에서 rvalue를 다시 **“prvalue(pure rvalue)”**, **“xvalue(eXpiring value)”**로 분류   
- 임시 객체(temporary)나 리터럴, 값 자체(“소멸될 값”)처럼 보통 식이 끝나면 사라지는 것.
- rvalue는 임시, 스택에 생성되어 식 종료 시 파괴.   
- 대입문의 오른쪽에서 주로 사용.
- 대입 대상이 될 수 없음(“모든 rvalue는 const” 개념과 유사, though C++11 moved semantics changed that). 
- 예) 리터럴(`42`), 산술 결과(`x+y`), 임시 객체(`std::string("tmp")`)   
   
## glValue (generalized lvalue)   
- lvalue + xvalue를 합친 것(둘 다 메모리 위치 지칭, 주소 취득 가능).   
- 즉, **메모리에 있는 객체를 지칭하지만**(lvalue), **xvalue**처럼 “만료될 수 있는” 특별한 경우도 합쳐 **“glvalue”**.   
- glvalue는 주소를 구할 수 있고, 객체의 “특정 식별자” 역할을 한다는 공통점.   
   
## prvalue (pure rvalue)  
- **prvalue**: “순수” 임시 → 일반적인 임시 객체(“아직 쓰지 않은”), ex. `(1+2)`, `std::string("tmp")`  
- **“진짜 일시적”**인 값. 스코프 끝나면 소멸하는 “임시 객체” 혹은 “계산 결과.”   
- 예)   
  1) **리터럴**: `123`, `'A'`, `3.14`, `true` (문자열 리터럴은 사실 char const(&)[6], 하지만 std::string("Hello")는 prvalue 임시)   
  2) **임시 객체**: `std::string("hello")` (함수 리턴 등)   
  3) **산술 연산 결과**: `(a + b)`, `(x < y)`
  4) `std::vector<int>(10)` → 임시 vector prvalue
- `(x + 1)`, `funcReturningByValue()` 의 **임시 반환 값**(C++17 이전은 prvalue, C++17 표준에선 + RVO 규칙이 좀 더…)  
- prvalue는 **“완전히 소멸될”** 임시. 특별한 조치를 하지 않으면 식 끝나고 사라짐.

   


## xvalue (expiring value)   
- **xvalue**: “만료 직전” 임시. 주로 `std::move(...)` 등으로 나타남 → “자원을 빼앗길 수 있는” 임시   
- **rvalue**처럼 보이나, **자원을(메모리 등) “이제 곧 소멸할”** 객체를 지칭.   
- 더 구체적으로, **“배타적 소유권”**을 가진 **임시 객체**.   
- 예)   
  1) `std::move(someObj)`: 이건 rvalue 캐스팅했지만, 실제로는 **someObj**의 자원을 이동할 수 있는 상태 → “만료되기 직전”인 상태   
  2) 함수가 **T&&**로 반환했을 때(특정 케이스)   
- xvalue는 C++11의 **Move semantics**와 연계되어, **move 연산자**를 호출하도록 설계.
- **xvalue**: ‘만료될’ rvalue, 주로 `std::move(obj)`가 생성  
   - `std::move(s)`: now it’s an rvalue expression, but specifically “expiring value” → xvalue  
   - “function returning T&&” (rare) → typically xvalue  
   - “pointer to subobject of an xvalue object” also xvalue


## 추가   
- 실제로 `std::move`(T&&)가 반환하는 것은 **xvalue**   
- 리터럴/임시 객체는 **prvalue**   
- **lvalue**는 예전부터의 “객체 이름 등”   
- xvalue가 glvalue와 rvalue 사이를 이어주는 특수 case


# 5. 왜 이런 구분이 필요한가?

- **Move semantics**:  
  - prvalue: 임시 객체 “논리적으로 새로 생성된” → can be moved from without affecting other usage  
  - xvalue: “already existing object but cast to rvalue ref, possibly can be moved from”  
- **Perfect forwarding**:  
  - C++ 표준이 어떤 상황에서 xvalue / prvalue를 구분해 “리소스 이동이 가능” or “임시 but not moveable”  
- **Lifetime extension**:  
  - Binding const ref to a prvalue extends lifetime  
  - xvalue might behave differently in some contexts


# 7. 요약   
3. **move/forward**:  
   - `std::move`: lvalue→xvalue 캐스팅 (move 가능)  
   - `std::forward`: universal reference 인자를 “원래 lvalue/rvalue”로 캐스팅
4. **메모리 관점**:  
   - prvalue 임시는 함수 끝나면 소멸, xvalue는 “기존 객체지만 소유권 빼앗길 준비된 상태”  
   - lvalue는 유효 범위 내에 안정된 저장소

**이러한 개념**은 **“모던 C++”**에서 **move semantics**, **perfect forwarding**, **함수 오버로딩** 등에 핵심 역할을 하며, 효율적인 라이브러리 설계(예: `std::vector`의 move 지원, `std::unique_ptr`)에 큰 기여를 합니다.


# TODO
# 2. move와 forward (C++11 이후)

C++11에서 **임시 객체**나 “더 이상 사용되지 않을 객체”를 
**효율적으로 이동**(move)할 수 있도록,  
- **`std::move`**: lvalue를 **rvalue로 캐스팅**해, 
move 생성자/연산자 등을 호출 가능하게 만듦  

- **`std::forward`**: **“완벽 전달(perfect forwarding)”** 
시나리오에서, T&&(universal reference) 인자를 
“원래의 value category”로 캐스팅하여 전달


- **사용처**: 템플릿 함수에서 “universal reference / forwarding reference”를 
받은 인자를,  
  - “**원래 lvalue였으면 lvalue로**, rvalue였으면 rvalue로”**** 전달하기 
  위함(perfect forwarding).  
- 예:
  ```cpp
  template<typename T, typename... Args>
  std::shared_ptr<T> make_shared_obj(Args&&... args)
  {
      return std::shared_ptr<T>(new T(std::forward<Args>(args)...));
  }
  ```


# 3. 보편 참조(Universal Reference) / 전달 참조(Forwarding Reference)

```cpp
template<typename T>
void func(T&& x) { ... }
```

- 만약 **T가** template 파라미터로 “type deduction”을 거치고, “`T&&`”가 
**“lvalue reference or rvalue reference”**로 동적으로 결정되는 경우 → 
이를 **Forwarding Reference**(C++ 표준) 또는 **Universal Reference**
(Scott Meyers 용어)라 부름.  
- **동작**:
  - 호출 시 인자가 lvalue이면 T = `MyType&`, => `T&&` => `MyType& &&` 
  => collapse to `MyType&`
  
  - 호출 시 인자가 rvalue이면 T = `MyType`, => `T&&` => `MyType&&`

---

# 4. 메모리 관점 
3. **move**: “소유권 이전” 개념. 실제 메모리 복사를 최소화하여, 자원의 포인터만 이동  
   - ex) std::vector move시 내부 포인터를 복사해 ownership만 이전, 원래 vector는 비워진 상태(혹은 unspecified)  
4. **forward**: 메모리 관점에서, “함수 인자가 lvalue인지 rvalue인지 구분” → rvalue이면 “move 가능한 객체”로 전달, lvalue이면 “그냥 lvalue ref”로 전달


# 6. 정리 문답
**Q**: “move를 하면 진짜 메모리 복사 안 하나요?”  
**A**: “move는 객체가 제공하는 move 생성자/연산자에 달려 있음.
 일반적으로 동적 자원의 ‘포인터만’ 이동하고, 
 원 객체에 nullptr 등으로 표시. 
 **실제 메모리 블록(힙)은 복사가 아닌 '소유권'만 이전**.”

**Q**: “forward와 move 차이?”  
**A**: “`std::move`는 ‘무조건 rvalue 캐스팅’, 
`std::forward`는 ‘템플릿에서 T&&(universal reference) 인자를 
원래 형태로 캐스팅(조건부 rvalue)’. Perfect forwarding용.”




## 각 개념에 대한 자세한 설명
### 2. std::move  
- **std::move**  
  - 단순히 lvalue를 rvalue reference로 "캐스팅"하는 함수입니다.  
  - 이를 통해 객체의 이동 생성자나 이동 대입 연산자를 호출할 수 있습니다. 복사 비용을 줄일 수 있습니다.
  - 주의할 점은 std::move를 호출한 후에도 원래 객체는 유효하지만, 이동된 객체는 "빈 상태"가 될 수 있다는 것입니다.


### 3. Move Semantics (이동 시맨틱스)  
- **이동 생성자/대입 연산자**  
  - 객체의 자원을 복사하지 않고, 소유권을 "이동"시켜 효율성을 높이는 기법입니다.  
  - 복사 생성자와 달리, 내부 데이터를 복사하지 않고 포인터 등 소유권을 이전합니다.
  - 위 코드의 `MyType` 클래스는 rvalue를 받으면 이동 생성자가 호출되어 내부 데이터를 std::move로 처리합니다.
  - 예제의 `MyString` 클래스에서는 이동 생성자를 통해 `ms1`의 리소스를 `ms3`로 이전하고, `ms1`은 null 상태가 됩니다.


### 4. 보편참조 (Universal/Forwarding Reference)  
- **정의**  
  - 템플릿 함수에서 `T&&` 형태로 선언된 매개변수는 전달된 값의 값
   범주(즉, lvalue인지 rvalue인지)에 따라 적절하게 동작합니다.
  - 이를 "보편참조" 또는 "forwarding reference"라고 부릅니다.
- **std::forward**  
  - 보편참조 매개변수를 다른 함수로 전달할 때, 원래의 lvalue/rvalue 특성을 그대로 유지하는 데 사용됩니다.  
    - 이를 통해 함수 템플릿이 호출된 함수의 오버로드를 올바르게 선택할 수 있게 도와줍니다.
  - 예제의 `forwardingFunction`에서 `std::forward<T>(arg)`를 사용하면, 인자로 전달된 값이 lvalue면 lvalue로, rvalue면 rvalue로 전달됩니다.


### 5. Reference (참조)  
- **lvalue reference**  
  - 변수의 별칭으로, 일반적으로 `T&`로 표현됩니다.
    - 변수에 대한 별칭입니다.
     예를 들어, `int& a = b;`에서 `a`는 `b`에 대한 lvalue reference입니다.
- **rvalue reference**  
  - rvalue에 대한 참조입니다. `std::string&& s`처럼 임시 객체나 이동된 객체를 참조할 때 사용됩니다.
  - 임시 객체나 이동 가능한 객체를 참조하는 것으로, `T&&`로 표현됩니다.
  - 이를 통해 move semantics를 구현할 수 있습니다.
