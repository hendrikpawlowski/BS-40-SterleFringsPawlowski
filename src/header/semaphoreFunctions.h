int down(int semid){
    struct sembuf sem_up;
    sem_up.sem_num = 0;
    sem_up.sem_op = 1;
    sem_up.sem_flg = 0;

    semop(semid, &sem_up, 1);

    return 0;
}

int up(int semid){
    struct sembuf sem_up;
    sem_up.sem_num = 0;
    sem_up.sem_op = -1;
    sem_up.sem_flg = 0;

    semop(semid, &sem_up, 1);

    return 0;
}