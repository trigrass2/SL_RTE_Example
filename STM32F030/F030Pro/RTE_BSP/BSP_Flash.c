#include "BSP_Flash.h"
#if RTE_USE_KVDB == 1
/**
 * Read data from flash.
 * @note This operation's units is word.
 *
 * @param addr flash address
 * @param buf buffer to store read data
 * @param size read bytes size
 *
 * @return result
 */
EfErrCode ef_port_read(uint32_t addr, uint32_t *buf, size_t size) {
    EfErrCode result = EF_NO_ERR;
	uint8_t *buf_8 = (uint8_t *)buf;
    /*copy from flash to ram */
    for (size_t i = 0; i < size; i++, addr ++, buf_8++) {
		while(FLASH_GetStatus()==FLASH_BUSY){};
        *buf_8 = *(uint8_t *) addr;
    }
    return result;
}
/**
 * Erase data on flash.
 * @note This operation is irreversible.
 * @note This operation's units is different which on many chips.
 *
 * @param addr flash address
 * @param size erase bytes size
 *
 * @return result
 */
EfErrCode ef_port_erase(uint32_t addr, size_t size) {
    EfErrCode result = EF_NO_ERR;
    FLASH_Status flash_status;
    size_t erase_pages, i;

    /* make sure the start address is a multiple of EF_ERASE_MIN_SIZE */
    RTE_AssertParam(addr % EF_ERASE_MIN_SIZE == 0);
    /* calculate pages */
    erase_pages = size / PAGE_SIZE;
    if (size % PAGE_SIZE != 0) {
        erase_pages++;
    }
    /* start erase */
    FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);
    /* it will stop when erased size is greater than setting size */
    for (i = 0; i < erase_pages; i++) {
		while(FLASH_GetStatus()==FLASH_BUSY){};
        flash_status = FLASH_ErasePage(addr + (PAGE_SIZE * i));
        if (flash_status != FLASH_COMPLETE) {
            result = EF_ERASE_ERR;
            break;
        }
    }
    FLASH_Lock();

    return result;
}
/**
 * Write data to flash.
 * @note This operation's units is word.
 * @note This operation must after erase. @see flash_erase.
 *
 * @param addr flash address
 * @param buf the write data buffer
 * @param size write bytes size
 *
 * @return result
 */
EfErrCode ef_port_write(uint32_t addr, const uint32_t *buf, size_t size) {
    EfErrCode result = EF_NO_ERR;
    size_t i;
    uint32_t read_data;

    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);
    for (i = 0; i < size; i += 4, buf++, addr += 4) {
		while(FLASH_GetStatus()==FLASH_BUSY){};
        /* write data */
        FLASH_ProgramWord(addr, *buf);
        read_data = *(uint32_t *)addr;
        /* check data */
        if (read_data != *buf) {
            result = EF_WRITE_ERR;
            break;
        }
    }
    FLASH_Lock();

    return result;
}
#else
/**
 * Read data from flash.
 * @note This operation's units is word.
 *
 * @param addr flash address
 * @param buf buffer to store read data
 * @param size read bytes size
 *
 * @return result
 */
EfErrCode ef_port_read(uint32_t addr, uint32_t *buf, size_t size) {
	EfErrCode result = EF_NO_ERR;
    const sfud_flash *flash = sfud_get_device_table() + SFUD_W25_DEVICE_INDEX;
    sfud_read(flash, addr, size, (uint8_t *)buf);
    return result;
}
/**
 * Erase data on flash.
 * @note This operation is irreversible.
 * @note This operation's units is different which on many chips.
 *
 * @param addr flash address
 * @param size erase bytes size
 *
 * @return result
 */
EfErrCode ef_port_erase(uint32_t addr, size_t size) {
    EfErrCode result = EF_NO_ERR;
    sfud_err sfud_result = SFUD_SUCCESS;
    const sfud_flash *flash = sfud_get_device_table() + SFUD_W25_DEVICE_INDEX;
    /* make sure the start address is a multiple of FLASH_ERASE_MIN_SIZE */
    RTE_AssertParam(addr % EF_ERASE_MIN_SIZE == 0);
    sfud_result = sfud_erase(flash, addr, size);
    if(sfud_result != SFUD_SUCCESS) {
        result = EF_ERASE_ERR;
    }
    return result;
}
/**
 * Write data to flash.
 * @note This operation's units is word.
 * @note This operation must after erase. @see flash_erase.
 *
 * @param addr flash address
 * @param buf the write data buffer
 * @param size write bytes size
 *
 * @return result
 */
EfErrCode ef_port_write(uint32_t addr, const uint32_t *buf, size_t size) {
    EfErrCode result = EF_NO_ERR;
    sfud_err sfud_result = SFUD_SUCCESS;
    const sfud_flash *flash = sfud_get_device_table() + SFUD_W25_DEVICE_INDEX;
    sfud_result = sfud_write(flash, addr, size, (const uint8_t *)buf);
    if(sfud_result != SFUD_SUCCESS) {
        result = EF_WRITE_ERR;
    }
    return result;
}
#endif
/**
 * lock the ENV ram cache
 */
void ef_port_env_lock(void) {
#if RTE_USE_OS == 1
	osMutexAcquire(MutexIDKVDB,osWaitForever);
#endif
}

/**
 * unlock the ENV ram cache
 */
void ef_port_env_unlock(void) {
#if RTE_USE_OS == 1
	osMutexRelease(MutexIDKVDB);
#endif
}
