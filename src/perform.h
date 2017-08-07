/*
 * perform.h
 *
 *  Created on: 2017. 8. 7.
 *      Author: root
 */

#ifndef SRC_PERFORM_H_
#define SRC_PERFORM_H_

#ifndef likely
    #define likely(x)       __builtin_expect((x),1)
#endif
#ifndef unlikely
    #define unlikely(x)     __builtin_expect((x),0)
#endif

#endif /* SRC_PERFORM_H_ */
