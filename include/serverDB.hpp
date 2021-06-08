#include <unordered_map>
#include <vector>
#include "message.hpp"
#include "tuple.hpp"

namespace linda{

class ServerDB{
public:
    ServerDB();
    bool addTupleToDB(std::vector<TupleElem> newTuple);
    std::vector<TupleElem> findTuple(std::vector<Pattern> pattern);
    std::vector<TupleElem> findTupleAndRemoveIt(std::vector<Pattern> pattern);

private:
    bool isTupleAlreadyInDatabase(std::vector<TupleElem> left, std::vector<TupleElem> right);

    std::unordered_map<int, std::vector<std::vector<TupleElem>>> records;
    pthread_mutex_t mutex;



    template<typename T>
    bool compareTupleElems(TupleElem left, TupleElem right){
        if( T* var_left = std::get_if<T>(&left) ){
            if(T* var_right = std::get_if<T>(&right)){
                if(*var_left != *var_right){
                    return false;
                }
            }
            else{
                return false;
            }
        }

        return true;
    }
};

} //namespace linda