#ifndef __MANAGER_H__
#define __MANAGER_H__
#include <stddef.h>  /* size_t */
/** 
 *  @file manager.h
 *  @brief Provides functionalities for retrieving data from a container of CDR's.
 * 
 *  @details Given a container that may be consisted of several different data structures,
 *  The following API provides functionalities to access each entry in each DS
 * 	and retrieve information about the entire DS.
 *
 *  @author Stav Klein (klein.stav@gmail.com) 
 * 
 */ 

typedef struct Container Container;
typedef void (*UpdateValue)(void* _originalValue, void* _toUpdateValue);

/** 
 * @brief 		Creates a container according to the number of CDR's
 * @param[in]  	_capacity: 	the amount of CDR's to be expected (will grow automatically if _capacity increases)
 * @return		A pointer to the allocated container
 */
Container* Mgr_CreateContainer(size_t _capacity);

/** 
 * @brief 		destroys container and set *_container to null
 * @param[in]  	_container: the container to be destroyed
 * @return		void
 */
void Mgr_DestroyContainer(Container** _container);

/** 
 * @brief 		Updates the relevant DS according to the given CDR, if the entry doesn't exist - creates it
 * @param[in]  	_container: the container that holds the CDR's
 * @param[in]  	_type: the type of DS to be inserted to. 0 for subscribers, 1 for operators
 * @return		1 on success, 0 otherwise
 */
int Mgr_UpsertSub(Container* _container, void* _structS, UpdateValue _updateFunc);
int Mgr_UpsertOp(Container* _container, void* _structO, UpdateValue _updateFunc);

/** 
 * @brief 		Gets the current record of one subscriber from the container, based on the subscriber's msisdn
 * @param[in]  	_container: the container that holds the CDR's
 * @param[in]  	_msisdn: the key by which to look for the subscriber (as a string)
 * @return		A pointer to the generated file with the given subscriber's entry. 
 *				The generated file will be named "sub_msisdn_output.txt"
 * @retval		0 if subscriber doesn't exist, 1 on success.
 */
int Mgr_GetOneSubscriber(Container* _container, char* _msisdn);

/** 
 * @brief 		Gets the current record of one operator from the container, based on the operator's MCC/MNC
 * @param[in]  	_container: the container that holds the CDR's
 * @param[in]  	_mcc_mnc: the key by which to look for the operator (as a string)
 * @return		A pointer to the generated file with the given operator's entry. 
 *				The generated file will be named "op_mcc_output.txt"
 * @retval		0 if operator doesn't exist 1 on success. 
 */
int Mgr_GetOneOperator(Container* _container, char* _mcc_mnc);

/** 
 * @brief		Gets the record of either all subscribers or all operators into a file 		
 * @param[in]  	_container: the container that holds the CDR's
 * @param[in]	_type: the type of the data to retrieve. 0 for subscribers, 1 for operators.
 * @return		0 if operator doesn't exist 1 on success. 
 */
int Mgr_ForEach(Container* _container, int _type);


#endif /* __MANAGER_H__ */
