#include <stdio.h>
#include <stdlib.h>
#include "redset.h"
#include "redset_util.h"
#include "redset_internal.h"

#include "kvtree.h"
#include "kvtree_util.h"

int
main(int argc, char *argv[]) {
    int rc;
    kvtree* redset_config_values = kvtree_new();

    int old_redset_debug = redset_debug;
    int old_redset_set_size = redset_set_size;
    int old_redset_mpi_buf_size = redset_mpi_buf_size;

    MPI_Init(&argc, &argv);

    rc = redset_init();
    if (rc != REDSET_SUCCESS) {
        printf("redset_init() failed (error %d)\n", rc);
        return rc;
    }

    /* check redset configuration settings */
    rc = kvtree_util_set_int(redset_config_values, REDSET_KEY_CONFIG_DEBUG,
                             !old_redset_debug);
    if (rc != KVTREE_SUCCESS) {
        printf("kvtree_util_set_int failed (error %d)\n", rc);
        return rc;
    }
    rc = kvtree_util_set_int(redset_config_values, REDSET_KEY_CONFIG_SET_SIZE,
                             old_redset_set_size + 1);
    if (rc != KVTREE_SUCCESS) {
        printf("kvtree_util_set_int failed (error %d)\n", rc);
        return rc;
    }
    rc = kvtree_util_set_int(redset_config_values, REDSET_KEY_CONFIG_MPI_BUF_SIZE,
                             old_redset_mpi_buf_size + 1);
    if (rc != KVTREE_SUCCESS) {
        printf("kvtree_util_set_int failed (error %d)\n", rc);
        return rc;
    }

    printf("Configuring redset...\n");
    rc = redset_config(redset_config_values);
    if (rc != REDSET_SUCCESS) {
        printf("redset_config() failed (error %d)\n", rc);
        return rc;
    }

    printf("Configuring redset a second time (this should fail)...\n");
    rc = redset_config(redset_config_values);
    if (rc == REDSET_SUCCESS) {
        printf("redset_config() succeeded unexpectedly (error %d)\n", rc);
        return rc;
    }

    if (redset_debug != !old_redset_debug) {
        printf("redset_config() failed to set %s: %d != %d\n",
               REDSET_KEY_CONFIG_DEBUG, redset_debug, !old_redset_debug);
        return EXIT_FAILURE;
    }

    if (redset_set_size != old_redset_set_size + 1) {
        printf("REDSET_Config() failed to set %s: %d != %d\n",
               REDSET_KEY_CONFIG_SET_SIZE, redset_set_size, old_redset_set_size);
        return EXIT_FAILURE;
    }

    if (redset_mpi_buf_size != old_redset_mpi_buf_size + 1) {
        printf("REDSET_Config() failed to set %s: %d != %d\n",
               REDSET_KEY_CONFIG_MPI_BUF_SIZE, redset_mpi_buf_size,
               old_redset_mpi_buf_size);
        return EXIT_FAILURE;
    }

    rc = redset_finalize();
    if (rc != REDSET_SUCCESS) {
        printf("redset_finalize() failed (error %d)\n", rc);
        return rc;
    }

    MPI_Finalize();

    return REDSET_SUCCESS;
}
