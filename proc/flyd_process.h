//Copyright(C) Ideal Dragon. All rights reserved.
//Use if this source code is governed by GPL-style license
//Author: Ideal Dragon
//
// Created by Administrator on 2019/7/13.
//

#ifndef PROCESS_FLYD_PROCESS_H
#define PROCESS_FLYD_PROCESS_H

namespace flyd {
    int flyd_daemon();

    void flyd_master_process_cycle();

    static void flyd_start_worker_processes(int processnums);

    static int flyd_spawn_process(int inum, const char *pprocname);

    static void flyd_worker_process_cycle(int inum, const char *pprocname);

    static void flyd_worker_process_init(int inum);
}

#endif //PROCESS_FLYD_PROCESS_H
