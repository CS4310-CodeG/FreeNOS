#ifndef __BIN_PRIME_SLOWCALC_H
#define __BIN_PRIME_SLOWCALC_H

#include <POSIXApplication.h>

/**
 * @addtogroup bin
 * @{
 */

/**
 * Computer prime numbers using the Sieve of Eratosthenes algorithm
 */
class SlowCalc : public POSIXApplication {
    public:

        /**
         * Constructor
         *
         * @param argc Argument count
         * @param argv Argument values
         */
        SlowCalc(int argc, char **argv);

        /**
         * Destructor
         */
        virtual ~SlowCalc();

        /**
         * Execute the application.
         *
         * @return Result code
         */
        virtual Result exec();
};

/**
 * @}
 */

#endif /* __BIN_PRIME_SLOWCALC_H */