#include "rtthread.h"
#include "NuMicro.h"                /* For CPU related defintiions */
//#include <inttypes.h>

//using namespace std;

/****************************************************************************
 * Mutex & Semaphore
 * Overrides weak-linked symbols in ethosu_driver.c to implement thread handling
 ****************************************************************************/
void ethosu_flush_dcache(uint32_t *p, size_t bytes)
{
    if (SCB->CCR & SCB_CCR_DC_Msk)
    {
        if (p && (bytes > 0))
            SCB_CleanDCache_by_Addr(p, bytes);
    }
}

void ethosu_invalidate_dcache(uint32_t *p, size_t bytes)
{
    if (SCB->CCR & SCB_CCR_DC_Msk)
    {
        if (p && (bytes > 0))
            SCB_InvalidateDCache_by_Addr(p, bytes);
    }
}

void *ethosu_mutex_create(void)
{
    rt_mutex_t mutex = rt_mutex_create("eu_mutex", RT_IPC_FLAG_PRIO);

    if (mutex == NULL)
    {
        rt_kprintf("Error: Failed to create mutex.\n");
        RT_ASSERT(0);
    }

    return (void *)mutex;
}

void ethosu_mutex_destroy(void *mutex)
{
    RT_ASSERT(mutex != RT_NULL);

    rt_mutex_delete(mutex);
}

int ethosu_mutex_lock(void *mutex)
{
    RT_ASSERT(mutex != RT_NULL);

    if (rt_mutex_take(mutex, RT_WAITING_FOREVER) != RT_EOK)
    {
        rt_kprintf("Error: Failed to lock mutex.\n");
        return -1;
    }

    return 0;
}

int ethosu_mutex_unlock(void *mutex)
{
    RT_ASSERT(mutex != RT_NULL);

    if (rt_mutex_release(mutex) != RT_EOK)
    {
        rt_kprintf("Error: Failed to unlock mutex.\n");
        return -1;
    }

    return 0;
}

void *ethosu_semaphore_create(void)
{
    /* Create binary semaphore. */
    rt_sem_t sem = rt_sem_create("eu_sema", 0, RT_IPC_FLAG_PRIO);

    if (sem == NULL)
    {
        rt_kprintf("Error: Failed to create semaphore.\n");
        RT_ASSERT(0);
    }

    return (void *)sem;
}

void ethosu_semaphore_destroy(void *sem)
{
    RT_ASSERT(sem != RT_NULL);

    rt_sem_delete(sem);
}

int ethosu_semaphore_take(void *sem, uint64_t timeout)
{
    RT_ASSERT(sem != RT_NULL);

    if (rt_sem_take(sem, (uint32_t)timeout) != RT_EOK)
    {
        rt_kprintf("Error: Failed to lock semaphore.\n");
        return -1;
    }

    return 0;
}

int ethosu_semaphore_give(void *sem)
{
    RT_ASSERT(sem != RT_NULL);

    if (rt_sem_release(sem) != RT_EOK)
    {
        rt_kprintf("Error: Failed to unlock semaphore.\n");
        return -1;
    }

    return 0;
}

