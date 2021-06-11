#include <variant>
#include <type_traits>
#include "server.hpp"
#include "common.hpp"

linda::ServerDB::ServerDB(){}

bool linda::ServerDB::addTupleToDB(std::vector<TupleElem> newTuple){
    LOG_S(INFO) << "Adding Tuple";

    if( informWaitingThreads(newTuple) ){
        LOG_S(INFO) << "Someone already has taken the tuple";
        return true;
    }

    {
        std::scoped_lock<std::mutex> lock(db_mutex);

        std::vector<std::vector<TupleElem>> curr_row =  records[newTuple.size()];
        for(auto iter = curr_row.begin(); iter != curr_row.end(); ++iter){
            std::vector<TupleElem> tuple = *iter;
            if( isTupleAlreadyInDatabase(tuple, newTuple)){
                LOG_S(INFO) << "This tuple is already on the server";
                return false;
            }
        }
        records[newTuple.size()].push_back(newTuple);
    }
    LOG_S(INFO) << "Tuple has been added to the database";

    return true;
}


std::vector<linda::TupleElem> linda::ServerDB::findTuple(std::vector<Pattern> pattern){
    LOG_S(INFO) << "Looking for given pattern";
    std::scoped_lock<std::mutex> lock(db_mutex);

    std::vector<std::vector<TupleElem>> curr_row = records[pattern.size()];
    for(auto iter = curr_row.begin(); iter != curr_row.end(); ++iter){
        if( isPatternEqualToTuple(pattern, *iter) ){
            LOG_S(INFO) << "Found matching tuple";
            return *iter;
        }

    }
    LOG_S(INFO) << "Correct tuple not found";
    return std::vector<TupleElem>();
}


std::vector<linda::TupleElem> linda::ServerDB::findTupleAndRemoveIt(std::vector<Pattern> pattern){
    LOG_S(INFO) << "Looking for given pattern";
    std::scoped_lock<std::mutex> lock(db_mutex);

    std::vector<std::vector<TupleElem>>& curr_row = records[pattern.size()];
    for(auto iter = curr_row.begin(); iter != curr_row.end(); ++iter){
        if( isPatternEqualToTuple(pattern, *iter) ){
            LOG_S(INFO) << "Matching tuple found and removed";
            auto tuple = *iter;
            curr_row.erase(iter);
            return tuple;
        }
    }
    LOG_S(INFO) << "Correct tuple not found";
    return std::vector<TupleElem>();
}

bool linda::ServerDB::informWaitingThreads(std::vector<TupleElem> tuple){
    std::scoped_lock<std::mutex> lock(queue_mutex);
    for(auto iter = waiting_threads_queue.begin(); iter != waiting_threads_queue.end(); ++iter){

        if( isPatternEqualToTuple( (*iter)->tuple_pattern, tuple) ){
            (*iter)->passed_tuple = tuple;
            (*iter)->mutex.unlock();
            if( (*iter)->isInput ){
                waiting_threads_queue.erase(iter);
                return true;
            }
            waiting_threads_queue.erase(iter);
            --iter;
        }
    }
    return false;
}

std::vector<linda::TupleElem> linda::ServerDB::waitForTuple(AwaitingParams& awaiting_params){
    {
        std::scoped_lock<std::mutex> lock(queue_mutex);
        waiting_threads_queue.push_back(&awaiting_params);
    }

    LOG_S(INFO) << "I'm waiting for the tuple";
    awaiting_params.mutex.lock();
    LOG_S(INFO) << "finally, let's get back to work";

    return awaiting_params.passed_tuple;
}

bool linda::ServerDB::isPatternEqualToTuple(std::vector<Pattern> pattern, std::vector<TupleElem> tuple){
    if(pattern.size() != tuple.size())
        return false;
    
    bool isTupleCorrect = true;
    for(std::size_t i = 0; i < pattern.size(); ++i){
        if( !pattern[i].matches(tuple[i]) )
            isTupleCorrect = false;
    }

    return isTupleCorrect;
}

bool linda::ServerDB::isTupleAlreadyInDatabase(std::vector<TupleElem> left, std::vector<TupleElem> right){
    for(std::size_t i = 0; i < left.size(); ++i ){
        if(!compareTupleElems<int>(left[i], right[i]))
            return false;
        if(!compareTupleElems<double>(left[i], right[i]))
            return false;
        if(!compareTupleElems<std::string>(left[i], right[i]))
            return false;
    }
    return true;
}