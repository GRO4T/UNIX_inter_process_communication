#include <variant>
#include <type_traits>
#include "server.hpp"
#include "common.hpp"

linda::ServerDB::ServerDB(){}

bool linda::ServerDB::addTupleToDB(std::vector<TupleElem> newTuple){
    LOG_S(INFO) << "Dodaje krotke";
    {
        std::scoped_lock<std::mutex> lock(db_mutex);

        std::vector<std::vector<TupleElem>> curr_row =  records[newTuple.size()];
        for(auto iter = curr_row.begin(); iter != curr_row.end(); ++iter){
            std::vector<TupleElem> tuple = *iter;
            if( isTupleAlreadyInDatabase(tuple, newTuple)){
                LOG_S(INFO) << "Taka krotka juz jest na serwerze";
                return false;
            }
        }
        records[newTuple.size()].push_back(newTuple);
    }
    LOG_S(INFO) << "Pomyslnie dodano krotke";

    informWaitingThreads(newTuple);

    return true;
}


std::vector<linda::TupleElem> linda::ServerDB::findTuple(std::vector<Pattern> pattern){
    LOG_S(INFO) << "Szukam krotki";
    std::scoped_lock<std::mutex> lock(db_mutex);

    std::vector<std::vector<TupleElem>> curr_row = records[pattern.size()];
    for(auto iter = curr_row.begin(); iter != curr_row.end(); ++iter){
        if( isPatternEqualToTuple(pattern, *iter) ){
            LOG_S(INFO) << "Znaleziono odpowiadajaca krotke";
            return *iter;
        }

    }
    LOG_S(INFO) << "Nie znaleziono odpowiadajacej krotki";
    return std::vector<linda::TupleElem>();
}


std::vector<linda::TupleElem> linda::ServerDB::findTupleAndRemoveIt(std::vector<Pattern> pattern){
    LOG_S(INFO) << "Szukam krotki";
    std::scoped_lock<std::mutex> lock(db_mutex);

    std::vector<std::vector<TupleElem>> curr_row = records[pattern.size()];
    for(auto iter = curr_row.begin(); iter != curr_row.end(); ++iter){
        if( isPatternEqualToTuple(pattern, *iter) ){
            LOG_S(INFO) << "Znaleziono i usunieto odpowiadajaca krotke";
            auto tuple = *iter;
            curr_row.erase(iter);
            return tuple;
        }

    }
    LOG_S(INFO) << "Nie znaleziono odpowiadajacej krotki";
    return std::vector<linda::TupleElem>();
}

void linda::ServerDB::informWaitingThreads(std::vector<TupleElem> tuple){
    std::scoped_lock<std::mutex> lock(queue_mutex);
    for(auto iter = awaitedTuples.begin(); iter != awaitedTuples.end() ; ++iter){
        auto curr_pattern = iter->first;
        if( isPatternEqualToTuple(curr_pattern, tuple) ){
            iter->second->unlock();
            awaitedTuples.erase(iter);
            return;
        }
    }
}

void linda::ServerDB::waitForTuple(std::vector<linda::Pattern> pattern){
    std::mutex newMutex;
    {
        std::scoped_lock<std::mutex> lock(queue_mutex);
        awaitedTuples.push_back(std::make_pair(pattern, &newMutex));
    }

    LOG_S(INFO) << "Czekam sobie na tuple :)";
    newMutex.lock();
    newMutex.lock();
    LOG_S(INFO) << "Koniec czekania, let's do this";
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