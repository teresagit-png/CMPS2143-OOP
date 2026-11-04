#include "Hand.hpp"

#include <stdexcept>

void Hand::addCard(const Card& c){ 
    cards.push_back(c); 
}

void Hand::clear(){
    cards.clear();
}
    
int Hand::size() const{
    return cards.size();
}

const Card& Hand::at(int i) const{
    if(i < 0 || i >= size()){
        throw std::out_of_range("Invalid card index");
    }
    return cards[i];
}

void Hand::replace(int i, const Card& c){
    if(i <0 || i >= size()){
        throw std::out_of_range("Invalid card index"); 
    }
    cards[i] = c;
}




