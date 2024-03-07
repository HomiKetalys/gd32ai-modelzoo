/**
  ******************************************************************************
  * @file    network_1.h
  * @author  AST Embedded Analytics Research Platform
  * @date    Wed Mar  6 23:34:56 2024
  * @brief   AI Tool Automatic Code Generator for Embedded NN computing
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  */

#ifndef AI_NETWORK_1_H
#define AI_NETWORK_1_H
#pragma once

#include "network_1_config.h"
#include "ai_platform.h"

/******************************************************************************/
#define AI_NETWORK_1_MODEL_NAME          "network_1"
#define AI_NETWORK_1_ORIGIN_MODEL_NAME   "model_front"

/******************************************************************************/
#define AI_NETWORK_1_ACTIVATIONS_ALIGNMENT   (4)
#define AI_NETWORK_1_INPUTS_IN_ACTIVATIONS   (4)
#define AI_NETWORK_1_OUTPUTS_IN_ACTIVATIONS  (4)

/******************************************************************************/
#define AI_NETWORK_1_IN_NUM        (1)

AI_DEPRECATED
#define AI_NETWORK_1_IN \
  ai_network_1_inputs_get(AI_HANDLE_NULL, NULL)

#define AI_NETWORK_1_IN_SIZE { \
  AI_NETWORK_1_IN_1_SIZE, \
}
#define AI_NETWORK_1_IN_SIZE_BYTES { \
  AI_NETWORK_1_IN_1_SIZE_BYTES, \
}
#define AI_NETWORK_1_IN_1_HEIGHT      (128)
#define AI_NETWORK_1_IN_1_WIDTH       (128)
#define AI_NETWORK_1_IN_1_CHANNEL     (3)
#define AI_NETWORK_1_IN_1_SIZE        (128 * 128 * 3)
#define AI_NETWORK_1_IN_1_SIZE_BYTES  (49153)

/******************************************************************************/
#define AI_NETWORK_1_OUT_NUM       (1)

AI_DEPRECATED
#define AI_NETWORK_1_OUT \
  ai_network_1_outputs_get(AI_HANDLE_NULL, NULL)

#define AI_NETWORK_1_OUT_SIZE { \
  AI_NETWORK_1_OUT_1_SIZE, \
}
#define AI_NETWORK_1_OUT_SIZE_BYTES { \
  AI_NETWORK_1_OUT_1_SIZE_BYTES, \
}
#define AI_NETWORK_1_OUT_1_HEIGHT      (8)
#define AI_NETWORK_1_OUT_1_WIDTH       (8)
#define AI_NETWORK_1_OUT_1_CHANNEL     (96)
#define AI_NETWORK_1_OUT_1_SIZE        (8 * 8 * 96)
#define AI_NETWORK_1_OUT_1_SIZE_BYTES  (6144)

/******************************************************************************/
#define AI_NETWORK_1_N_NODES (89)


AI_API_DECLARE_BEGIN

/*!
 * @defgroup network_1
 * @brief Public neural network APIs
 * @details This is the header for the network public APIs declarations
 * for interfacing a generated network model.
 * @details The public neural network APIs hide the structure of the network
 * and offer a set of interfaces to create, initialize, query, configure, 
 * run and destroy a network instance.
 * To handle this, an opaque handler to the network context is provided 
 * on creation.
 * The APIs are meant as stadard interfaces for the calling code; depending on
 * the supported platforms and the models, different implementations could be
 * available.
 */

/******************************************************************************/
/*! Public API Functions Declarations */

/*!
 * @brief Get network library info as a datastruct.
 * @ingroup network_1
 * @param[in] network: the handler to the network context
 * @param[out] report a pointer to the report struct where to
 * store network info. See @ref ai_network_report struct for details
 * @return a boolean reporting the exit status of the API
 */
AI_DEPRECATED
AI_API_ENTRY
ai_bool ai_network_1_get_info(
  ai_handle network, ai_network_report* report);


/*!
 * @brief Get network library report as a datastruct.
 * @ingroup network_1
 * @param[in] network: the handler to the network context
 * @param[out] report a pointer to the report struct where to
 * store network info. See @ref ai_network_report struct for details
 * @return a boolean reporting the exit status of the API
 */
AI_API_ENTRY
ai_bool ai_network_1_get_report(
  ai_handle network, ai_network_report* report);

/*!
 * @brief Get first network error code.
 * @ingroup network_1
 * @details Get an error code related to the 1st error generated during
 * network processing. The error code is structure containing an 
 * error type indicating the type of error with an associated error code
 * Note: after this call the error code is internally reset to AI_ERROR_NONE
 * @param network an opaque handle to the network context
 * @return an error type/code pair indicating both the error type and code
 * see @ref ai_error for struct definition
 */
AI_API_ENTRY
ai_error ai_network_1_get_error(ai_handle network);

/*!
 * @brief Create a neural network.
 * @ingroup network_1
 * @details Instantiate a network and returns an object to handle it;
 * @param network an opaque handle to the network context
 * @param network_config a pointer to the network configuration info coded as a 
 * buffer
 * @return an error code reporting the status of the API on exit
 */
AI_API_ENTRY
ai_error ai_network_1_create(
  ai_handle* network, const ai_buffer* network_config);

/*!
 * @brief Destroy a neural network and frees the allocated memory.
 * @ingroup network_1
 * @details Destroys the network and frees its memory. The network handle is returned;
 * if the handle is not NULL, the unloading has not been successful.
 * @param network an opaque handle to the network context
 * @return an object handle : AI_HANDLE_NULL if network was destroyed
 * correctly. The same input network handle if destroy failed.
 */
AI_API_ENTRY
ai_handle ai_network_1_destroy(ai_handle network);

/*!
 * @brief Initialize the data structures of the network.
 * @ingroup network_1
 * @details This API initialized the network after a successfull
 * @ref ai_network_1_create. Both the activations memory buffer 
 * and params (i.e. weights) need to be provided by caller application
 * 
 * @param network an opaque handle to the network context
 * @param params the parameters of the network (required). 
 * see @ref ai_network_params struct for details
 * @return true if the network was correctly initialized, false otherwise
 * in case of error the error type could be queried by 
 * using @ref ai_network_1_get_error
 */
AI_API_ENTRY
ai_bool ai_network_1_init(
  ai_handle network, const ai_network_params* params);

/*!
 * @brief Create and initialize a neural network (helper function)
 * @ingroup network_1
 * @details Helper function to instantiate and to initialize a network. It returns an object to handle it;
 * @param network an opaque handle to the network context
 * @param activations array of addresses of the activations buffers
 * @param weights array of addresses of the weights buffers
 * @return an error code reporting the status of the API on exit
 */
AI_API_ENTRY
ai_error ai_network_1_create_and_init(
  ai_handle* network, const ai_handle activations[], const ai_handle weights[]);

/*!
 * @brief Get network inputs array pointer as a ai_buffer array pointer.
 * @ingroup network_1
 * @param network an opaque handle to the network context
 * @param n_buffer optional parameter to return the number of outputs
 * @return a ai_buffer pointer to the inputs arrays
 */
AI_API_ENTRY
ai_buffer* ai_network_1_inputs_get(
  ai_handle network, ai_u16 *n_buffer);

/*!
 * @brief Get network outputs array pointer as a ai_buffer array pointer.
 * @ingroup network_1
 * @param network an opaque handle to the network context
 * @param n_buffer optional parameter to return the number of outputs
 * @return a ai_buffer pointer to the outputs arrays
 */
AI_API_ENTRY
ai_buffer* ai_network_1_outputs_get(
  ai_handle network, ai_u16 *n_buffer);

/*!
 * @brief Run the network and return the output
 * @ingroup network_1
 *
 * @details Runs the network on the inputs and returns the corresponding output.
 * The size of the input and output buffers is stored in this
 * header generated by the code generation tool. See AI_NETWORK_1_*
 * defines into file @ref network_1.h for all network sizes defines
 *
 * @param network an opaque handle to the network context
 * @param[in] input buffer with the input data
 * @param[out] output buffer with the output data
 * @return the number of input batches processed (default 1) or <= 0 if it fails
 * in case of error the error type could be queried by 
 * using @ref ai_network_1_get_error
 */
AI_API_ENTRY
ai_i32 ai_network_1_run(
  ai_handle network, const ai_buffer* input, ai_buffer* output);

/*!
 * @brief Runs the network on the inputs.
 * @ingroup network_1
 *
 * @details Differently from @ref ai_network_run, no output is returned, e.g. for
 * temporal models with a fixed step size.
 *
 * @param network the network to be run
 * @param[in] input buffer with the input data
 * @return the number of input batches processed (usually 1) or <= 0 if it fails
 * in case of error the error type could be queried by 
 * using @ref ai_network_1_get_error
 */
AI_API_ENTRY
ai_i32 ai_network_1_forward(
  ai_handle network, const ai_buffer* input);

AI_API_DECLARE_END

#endif /* AI_NETWORK_1_H */
