#include <unordered_map>
#include <vector>
#include "message.hpp"
#include "tuple.hpp"
#include "service_thread.hpp"

namespace linda{

class ServerDB{
public:
    ServerDB();
    bool addTupleToDB(std::vector<TupleElem> newTuple);
    std::vector<TupleElem> findTuple(std::vector<Pattern> pattern);
    std::vector<TupleElem> findTupleAndRemoveIt(std::vector<Pattern> pattern);
    std::vector<TupleElem> waitForTuple(AwaitingParams& awaiting_params);

private:
    bool isTupleAlreadyInDatabase(std::vector<TupleElem> left, std::vector<TupleElem> right);
    bool isPatternEqualToTuple(std::vector<Pattern> pattern,std::vector<TupleElem> tuple);
    bool informWaitingThreads(std::vector<TupleElem> tuple);

    std::unordered_map<int, std::vector<std::vector<TupleElem>>> records;
    std::vector<AwaitingParams*> waiting_threads_queue;

    std::mutex db_mutex;
    std::mutex queue_mutex;


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