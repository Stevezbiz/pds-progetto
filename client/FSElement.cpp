//
// Created by stevezbiz on 02/11/20.
//

#include "FSElement.h"
#include <iostream>
#include <utility>
#include "../comm/Utils.h"

namespace fs = boost::filesystem;

/**
 * Inizializza i campi dell'oggetto e ne calcola anche l'hash
 * @param path: il path del file/directory
 * @param type: il tipo dell'elemento (file/directory)
 * @param last_write_time: l'ultima modifica del file
 */
FSElement::FSElement(fs::path path, time_t last_write_time) : path_(std::move(path)),
                                                              last_write_time_(last_write_time) {
    hash_ = Utils::SHA256(path_);
}

/**
 * Verifica se un elemento è stato aggiornato dall'ultimo check e riscrive la data di ultima modifica
 * La modifica effettiva però è verificata dalla funzione needUpdate()
 * @param lwt: data di ultima modifica effettiva
 * @return True se l'elemento non è aggiornato
 */
bool FSElement::isOld(time_t lwt) {
    if (last_write_time_ == lwt) {
        return false;
    } else {
        last_write_time_ = lwt;
        return true;
    }
}

/**
 * Ricalcola l'hash dell'elemento e verifica così se l'elemento è stato effettivamente modificato
 * @return True se l'elemento è stato modificato
 */
bool FSElement::needUpdate() {
    std::string old = hash_;
    hash_ = Utils::SHA256(path_);
    if (old != hash_) {
        return true;
    } else {
        return false;
    }
}
