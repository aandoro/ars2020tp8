# Compilar

para compilar el programa usar:
```
mpicc vecesc_mpi.c -o vecesc_mpi -lm
```

# Correr
para correr la aplicacion usar:
```
mpirun -np <num_procesos> vecesc_mpi
```
yo use 2 porque mi maquina no me permitia mas