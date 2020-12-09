//
// Created by stevezbiz on 01/11/2020.
//

#ifndef BOOST_FS_FILEWATCHER_H
#define BOOST_FS_FILEWATCHER_H

#include <string>
#include <chrono>
#include <unordered_map>
#include <functional>
#include <thread>
#include <boost/filesystem.hpp>
#include <unordered_set>
#include "FSElement.h"
#include "../comm/ElementStatus.h"

class FileWatcher {
    std::string path_to_watch_;
    std::chrono::duration<int, std::milli> delay_;
    std::unordered_map<std::string, FSElement> files_;
    std::unordered_set<std::string> dirs_;
    bool running_;
    int path_offset_;

    /**
     * Verifica se la mappa contiene la chiave
     * @param key: chiave da cercare
     * @return True se la chiave è presente
     */
    bool contains(const std::string &key);

    /**
     * Verifica se gli elementi presenti al precedente check sono stati cancellati
     * @param action: una funzione di handler per gestire le eventuali cancellazioni
     */
    void findErased(const std::function<void(std::string, std::string hash, ElementStatus)> &action);

    /**
     * Verifica se gli elementi ora presenti non esistevano ancora al precedente check
     * e verifica se gli elementi sono stati modificati
     * @param action: una funzione di handler per gestire le eventuali modifiche riscontrate
     */
    void findCreatedOrModified(const std::function<void(std::string, std::string hash, ElementStatus)> &action);

public:
    /**
     * Inizializza i campi dell'oggetto e acquisisce la struttura iniziale della directory
     * @param path_to_watch: la directory che il file watcher deve monitorare
     * @param delay: il tempo di polling tra un check e il successivo
     */
    FileWatcher(std::string path_to_watch, std::chrono::duration<int, std::milli> delay);

    FileWatcher(const FileWatcher &) = delete;

    FileWatcher &operator=(const FileWatcher &) = delete;

    /**
     * Opera il polling sul thread del file watcher e chiama le funzioni per il check delle modifiche alla directory
     * @param action: una funzione di handler per gestire le eventuali modifiche riscontrate
     */
    void start(const std::function<void(std::string, std::string hash, ElementStatus)> &action);

    /**
     *
     */
    void init();

    /**
     *
     * @return
     */
    std::map<std::string, std::string> get_files();

    /**
     *
     * @param path
     * @return
     */
    std::string parse_path(const boost::filesystem::path &path) const;
};


#endif //BOOST_FS_FILEWATCHER_H
