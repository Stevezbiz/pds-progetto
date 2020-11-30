//
// Created by stevezbiz on 02/11/20.
//

#ifndef BOOST_FS_FSELEMENT_H
#define BOOST_FS_FSELEMENT_H

#include <ctime>
#include <string>
#include <boost/filesystem.hpp>
#include <iostream>
#include "../comm/Utils.h"

class FSElement {
    boost::filesystem::path path_;
    time_t last_write_time_;
    std::string hash_;

public:
    /**
     * Inizializza i campi dell'oggetto e ne calcola anche l'hash
     * @param path: il path del file/directory
     * @param type: il tipo dell'elemento (file/directory)
     * @param last_write_time: l'ultima modifica del file
     */
    FSElement(const boost::filesystem::path& path, time_t last_write_time);

    /**
     * Verifica se un elemento è stato aggiornato dall'ultimo check e riscrive la data di ultima modifica
     * La modifica effettiva però è verificata dalla funzione needUpdate()
     * @param lwt: data di ultima modifica effettiva
     * @return True se l'elemento non è aggiornato
     */
    bool isOld(time_t lwt);

    /**
     * Ricalcola l'hash dell'elemento e verifica così se l'elemento è stato effettivamente modificato
     * @return True se l'elemento è stato modificato
     */
    bool needUpdate();
    /**
     * Getter method
     * @return the hash value
     */
    std::string getHash();
};


#endif //BOOST_FS_FSELEMENT_H
