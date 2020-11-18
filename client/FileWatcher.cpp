//
// Created by stevezbiz on 01/11/2020.
//

#include "FileWatcher.h"
#include <thread>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

/**
 * Inizializza i campi dell'oggetto e acquisisce la struttura iniziale della directory
 * @param path_to_watch: la directory che il file watcher deve monitorare
 * @param delay: il tempo di polling tra un check e il successivo
 */
FileWatcher::FileWatcher(std::string path_to_watch, std::chrono::duration<int, std::milli> delay) :
        path_to_watch_(std::move(path_to_watch)), delay_(delay), running_(true) {
    for (const auto &el : fs::recursive_directory_iterator(path_to_watch_)) {
        if (fs::is_directory(el.path())) {
            FSElement dir{el.path(), ElementType::dir, fs::last_write_time(el.path())};
            paths_.insert(std::make_pair(el.path().string(), dir));
        } else {
            FSElement file{el.path(), ElementType::file, fs::last_write_time(el.path())};
            paths_.insert(std::make_pair(el.path().string(), file));
        }
    }
}

/**
 * Opera il polling sul thread del file watcher e chiama le funzioni per il check delle modifiche alla directory
 * @param action: una funzione di handler per gestire le eventuali modifiche riscontrate
 */
void FileWatcher::start(const std::function<void(std::string, ElementStatus)> &action) {
    while (running_) {
        std::this_thread::sleep_for(delay_);
        findErased(action);
        findCreatedOrModified(action);
    }
}

/**
 * Verifica se la mappa contiene la chiave
 * @param key: chiave da cercare
 * @return True se la chiave è presente
 */
bool FileWatcher::contains(const std::string &key) {
    auto el = paths_.find(key);
    return el != paths_.end();
}

/**
 * Verifica se gli elementi presenti al precedente check sono stati cancellati
 * @param action: una funzione di handler per gestire le eventuali cancellazioni
 */
void FileWatcher::findErased(const std::function<void(std::string, ElementStatus)> &action) {
    auto it = paths_.begin();
    while (it != paths_.end()) {
        if (!fs::exists(it->first)) {
            if (it->second.getType() == ElementType::dir) {
                action(it->first, ElementStatus::erasedDir);
            } else {
                action(it->first, ElementStatus::erasedFile);
            }
            it = paths_.erase(it);
        } else {
            it++;
        }
    }
}

/**
 * Verifica se gli elementi ora presenti non esistevano ancora al precedente check
 * e verifica se gli elementi sono stati modificati
 * @param action: una funzione di handler per gestire le eventuali modifiche riscontrate
 */
void FileWatcher::findCreatedOrModified(const std::function<void(std::string, ElementStatus)> &action) {
    for (const auto &el : fs::recursive_directory_iterator(path_to_watch_)) {
        time_t lwt = fs::last_write_time(el);
        if (!contains(el.path().string())) {
            if (fs::is_directory(el)) {
                paths_.insert(std::make_pair(el.path().string(), FSElement{el.path(), ElementType::dir, lwt}));
                action(el.path().string(), ElementStatus::createdDir);
            } else {
                paths_.insert(std::make_pair(el.path().string(), FSElement{el.path(), ElementType::file, lwt}));
                action(el.path().string(), ElementStatus::createdFile);
            }
        } else if (fs::is_regular_file(el)) {
            if (paths_.find(el.path().string())->second.isOld(lwt)) {
                if (paths_.find(el.path().string())->second.needUpdate()) {
                    action(el.path().string(), ElementStatus::modifiedFile);
                }
            }
        }
    }
}
