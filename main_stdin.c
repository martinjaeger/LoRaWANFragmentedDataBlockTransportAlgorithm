#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "frag.h"
#include "bitmap.h"

int flash_write(uint32_t addr, uint8_t *buf, uint32_t len);
int flash_read(uint32_t addr, uint8_t *buf, uint32_t len);

#define FRAG_NB                 (1907U)
#define FRAG_SIZE               (232U)
#define FRAG_CR                 (FRAG_NB + 10U)
#define FRAG_TOLERENCE          (10U + FRAG_NB * (FRAG_PER + 5U) / 100U)
#define FRAG_PER                (20U)

#define DEC_BUF_SIZE ((                             \
    (BM_UNIT - 1) * 5 +                             \
    FRAG_NB * 2 +                                   \
    FRAG_TOLERENCE * (FRAG_TOLERENCE + 5) / 2) /    \
    BM_UNIT * sizeof(bm_t) +                        \
    FRAG_SIZE * 2 + 7 * 4) // alignment

frag_dec_t decobj;
uint8_t dec_buf[DEC_BUF_SIZE];
uint8_t dec_flash_buf[(FRAG_NB + FRAG_CR) * FRAG_SIZE + 1024*1024];

int flash_write(uint32_t addr, uint8_t *buf, uint32_t len)
{
    memcpy(dec_flash_buf + addr, buf, len);
    return 0;
}

int flash_read(uint32_t addr, uint8_t *buf, uint32_t len)
{
    memcpy(buf, dec_flash_buf + addr, len);
    return 0;
}

void putbuf(uint8_t *buf, int len)
{
    int i;
    for (i = 0; i < len; i++) {
        printf("%02X ", buf[i]);
    }
    printf("\n");
}

int main()
{
    int ret, len, nb;
    uint8_t buf[FRAG_SIZE];

    decobj.cfg.dt = dec_buf;
    decobj.cfg.maxlen = sizeof(dec_buf);
    decobj.cfg.nb = FRAG_NB;
    decobj.cfg.size = FRAG_SIZE;
    decobj.cfg.tolerence = FRAG_TOLERENCE;
    decobj.cfg.frd_func = flash_read;
    decobj.cfg.fwr_func = flash_write;
    len = frag_dec_init(&decobj);
    printf("memory cost: %d, nb %d, size %d, tol %d\n",
           len,
           decobj.cfg.nb,
           decobj.cfg.size,
           decobj.cfg.tolerence);

    printf("sizeof(dec_buf) = %d\n", sizeof(dec_buf));

    freopen(NULL, "rb", stdin);
    FILE *outfile = fopen("../../app_update_decoded_new_algo.bin", "wb");

    nb = 0;
    do {
        nb++;
        len = fread(buf, 1, sizeof(buf), stdin);
        if (nb % 10 != 0) {
            // skip each 10th fragment
            ret = frag_dec(&decobj, nb, buf, sizeof(buf));
            printf("frag %d, process status: %d\n", nb, ret);
        }
    } while (len > 0);

    fwrite(dec_flash_buf, 1, sizeof(dec_flash_buf), outfile);

    fclose(stdin);
    fclose(outfile);
}

