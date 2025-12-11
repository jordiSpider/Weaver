#ifndef RUN_MODE_H_
#define RUN_MODE_H_


/**
 * @enum RunMode
 * @brief Specifies the different modes in which a simulation can be executed.
 *
 * This enumeration is used to determine how the simulation should be
 * initialized and run.
 */
enum class RunMode
{
    FromConfig,         /**< Run the simulation using the configuration file as input. */
    FromTextCheckpoint, /**< Run the simulation by restoring from a text checkpoint. */
    FromBinaryCheckpoint/**< Run the simulation by restoring from a binary checkpoint. */
};


#endif // RUN_MODE_H_