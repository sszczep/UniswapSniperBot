#pragma once

#include <secp256k1_recovery.h>

extern "C" {
  #include <KeccakSponge.h>
}

#include "utils.hpp"
#include "rlp.hpp"

using namespace Utils;

namespace Transaction {
  /**
   * @brief Struct holding required transaction values
   * 
   */
  struct ETHValues {
    const char *nonce;
    const char *gasPrice;
    const char *gasLimit;
    const char *to;
    const char *data;
    const char *value;
  };

  /**
   * @brief Class for creating and signing Ethereum transactions
   * 
   */
  class ETHTransaction {
    /**
     * @brief Object holding transaction data and encoding it
     * 
     */
    RLP<> mRLP;

    /**
     * @brief secp256k1 context, it's created in constructor as it takes some time to initialize
     * 
     */
    secp256k1_context *mSecp256k1Context;

    public:

    /**
     * @brief Construct a new TX object
     * 
     */
    ETHTransaction();

    /**
     * @brief Sets transaction's values
     * 
     * @param transactionValues
     */
    void setValues(ETHValues *transactionValues);

    /**
     * @brief Calculates transaction's signature
     * 
     * @param privateKey private key buffer
     * @param signature output signature
     * @param recid output recovery id
     */
    void getSignature(Byte privateKey[32], Byte signature[64], int *recid);

    /**
     * @brief Signs transaction
     * 
     * @param privateKey private key buffer
     */
    void sign(Byte privateKey[32]);

    /**
     * @brief Get the signed transaction's buffer
     * 
     * @return buffer start pointer and its length
     */
    std::pair<Buffer, size_t> getBuffer();

    /**
     * @brief Get the hexadecimal representation of the transaction's buffer
     * 
     * @param output output string
     * @param nullTerminated should string be null terminated, defaults to false
     * @return pointer to the last char
     */
    char *getHexString(char *output, bool nullTerminated);
  };
}