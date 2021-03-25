#include <iostream>
#include "sorted_Effective_Vector.h"


void unit_test_for_unsorted_vector()
{
    Effective_Vector<int>  initilized_Vector{ 1,2,3,5,8,9,0,1 };
    assert(initilized_Vector.size() == 8);
    assert(!initilized_Vector.empty());

    for (auto& it : initilized_Vector)
    {
        std::cout << it << " ";
    }
    std::cout << std::endl;

    Effective_Vector<int> vect;
    vect.push_back(9);
    vect.push_back(1);
    vect.push_back(2);
    vect.push_back(5);
    vect.push_back(9);

    auto itBeg = vect.begin();
    assert(*itBeg == 9);
    assert(*(++itBeg) == 1);
    assert(*(++itBeg) == 2);
    assert(*(++itBeg) == 5);
    assert(*(++itBeg) == 9);
    
    itBeg = vect.begin();
    *itBeg = 50;


    assert(*vect.begin() == 50);
    assert(vect.front() == 50);
    vect.push_back(40);
    assert(vect.back() == 40);

    vect.pop_back(); // 40
    vect.pop_back(); // 9
    assert(vect.back() == 5);
    vect.pop_back(); // 5
    assert(vect.back() == 2);

    try
    {

        /*Effective_Vector<int> emptyVector;
        auto itBegin = emptyVector.begin();
        ++itBegin;*/

        // auto ten = vect[10];
        
        auto ten = vect.at(10);
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
    }


    int cap = vect.capacity();
    vect.clear();
    assert(!vect.size());
    assert(vect.empty());
    cap = vect.capacity();
}

void unit_test_for_sorted_vector()
{
    Effective_Vector<int, true> sorted_Vector;
    sorted_Vector.push_back(10);
    auto itBegin = sorted_Vector.begin();
    assert(*itBegin == 10, "sorted_vector.begin() = 10/n");

    sorted_Vector.push_back(1);
    sorted_Vector.push_back(3);
    sorted_Vector.push_back(2);

    sorted_Vector.push_back(31);
    sorted_Vector.push_back(28);
    sorted_Vector.push_back(10);

    assert(sorted_Vector[0] == 1);
    assert(sorted_Vector[1] == 2);
    assert(sorted_Vector[2] == 3);
    assert(sorted_Vector[3] == 10);
    assert(sorted_Vector.at(4) == 10);
    assert(sorted_Vector.at(5) == 28);
    assert(sorted_Vector.at(6) == 31);

    for (auto& it : sorted_Vector)
    {
        std::cout << it << " ";
    }
    std::cout << std::endl;
    assert(sorted_Vector.size() == 7, "sorted vector size = 7/n");

    assert(sorted_Vector.back() == 31);
    sorted_Vector.pop_back();
    assert(sorted_Vector.back() == 28);
    sorted_Vector.pop_back();
    assert(sorted_Vector.back() == 10);
    sorted_Vector.push_back(-2);
    assert(sorted_Vector.front() == -2);
    assert(sorted_Vector.back() == 10);

    assert(sorted_Vector[0] == -2);
    assert(sorted_Vector[1] == 1);
    assert(sorted_Vector[2] == 2);
    assert(sorted_Vector[3] == 3);
    assert(sorted_Vector[4] == 10);
    assert(sorted_Vector.at(5) == 10);


    Effective_Vector<char, true> char_sorted_Vector;
    char_sorted_Vector.push_back('b');
    char_sorted_Vector.push_back('a');
    char_sorted_Vector.push_back('f');
    assert(char_sorted_Vector[0] == 'a');
    assert(char_sorted_Vector[1] == 'b');
    assert(char_sorted_Vector.at(2) == 'f');

    try
    {
        sorted_Vector.at(15);
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
    }
}


int main()
{
    std::cout << "Test unsorted vector " << std::endl;
    unit_test_for_unsorted_vector();
  
    std::cout << "Test sorted vector " << std::endl;
    unit_test_for_sorted_vector();
    
    system("pause");
    return 0;
}