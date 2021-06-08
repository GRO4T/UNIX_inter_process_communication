#include <variant>
#include <type_traits>
#include "server.hpp"
#include "common.hpp"

linda::ServerDB::ServerDB(){
    mutex = PTHREAD_MUTEX_INITIALIZER;
}

bool linda::ServerDB::addTupleToDB(std::vector<TupleElem> newTuple){
    LOG_S(INFO) << "Dodaje krotke";
    pthread_mutex_lock(&mutex);

    std::vector<std::vector<TupleElem>> curr_row =  records[newTuple.size()];
    for(auto iter = curr_row.begin(); iter != curr_row.end(); ++iter){
        std::vector<TupleElem> tuple = *iter;
        if( isTupleAlreadyInDatabase(tuple, newTuple)){
            pthread_mutex_unlock(&mutex);
            LOG_S(INFO) << "Taka krotka juz jest na serwerze";
            return false;
        }
    }

    records[newTuple.size()].push_back(newTuple);
    LOG_S(INFO) << "Pomyslnie dodano krotke";
    pthread_mutex_unlock(&mutex);
    return true;
}


std::vector<linda::TupleElem> linda::ServerDB::findTuple(std::vector<Pattern> pattern){
    LOG_S(INFO) << "Szukam krotki";
    pthread_mutex_lock(&mutex);

    std::vector<std::vector<TupleElem>> curr_row = records[pattern.size()];
    for(auto iter = curr_row.begin(); iter != curr_row.end(); ++iter){
        std::vector<TupleElem> tuple = *iter;
        bool isTupleCorrect = true;
        for(int i = 0; i < pattern.size(); ++i){
            if( !pattern[i].matches(tuple[i]) )
                isTupleCorrect = false;
        }

        if( isTupleCorrect ){
            LOG_S(INFO) << "Znaleziono odpowiadajaca krotke";
            pthread_mutex_unlock(&mutex);
            return tuple;
        }

    }
    LOG_S(INFO) << "Nie znaleziono odpowiadajacej krotki";
    pthread_mutex_unlock(&mutex);
    return std::vector<linda::TupleElem>();
}


std::vector<linda::TupleElem> linda::ServerDB::findTupleAndRemoveIt(std::vector<Pattern> pattern){
    LOG_S(INFO) << "Szukam krotki";
    pthread_mutex_lock(&mutex);

    std::vector<std::vector<TupleElem>> curr_row = records[pattern.size()];
    for(auto iter = curr_row.begin(); iter != curr_row.end(); ++iter){
        std::vector<TupleElem> tuple = *iter;
        bool isTupleCorrect = true;
        for(int i = 0; i < pattern.size(); ++i){
            if( !pattern[i].matches(tuple[i]) )
                isTupleCorrect = false;
        }

        if( isTupleCorrect ){
            LOG_S(INFO) << "Znaleziono i usunieto odpowiadajaca krotke";
            curr_row.erase(iter);
            pthread_mutex_unlock(&mutex);
            return tuple;
        }

    }
    LOG_S(INFO) << "Nie znaleziono odpowiadajacej krotki";
    pthread_mutex_unlock(&mutex);
    return std::vector<linda::TupleElem>();
}

bool linda::ServerDB::isTupleAlreadyInDatabase(std::vector<TupleElem> left, std::vector<TupleElem> right){
    for(long unsigned int i = 0; i < left.size(); ++i ){
        if(!compareTupleElems<int>(left[i], right[i]))
            return false;
        if(!compareTupleElems<double>(left[i], right[i]))
            return false;
        if(!compareTupleElems<std::string>(left[i], right[i]))
            return false;
    }
    return true;
}