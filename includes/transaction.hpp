#pragma once

#include <secp256k1_recovery.h>

extern "C" {
  #include <KeccakSponge.h>
}

#include "utils.hpp"
#include "rlp.hpp"

/**
 * @brief Functions for Ethereum transactions.
 * Currently only supports mainnet.
 */
namespace Transaction {
  /**
   * @brief secp256k1 context, preinitialized as it's very slow to create.
   * 
   * The purpose of context structures is to cache large precomputed data tables
   * that are expensive to construct, and also to maintain the randomization data
   * for blinding.
   *
   * Do not create a new context object for each operation, as construction is
   * far slower than all other API calls (~100 times slower than an ECDSA
   * verification).
   */
  inline const secp256k1_context * const secp256k1Context { secp256k1_context_create(SECP256K1_CONTEXT_SIGN) };

  /**
   * @brief Maximum quantity buffer size.
   * 32 bytes is maximum EVM integer size.
   */
  inline const std::size_t quantityBufferSize = 32;

  /**
   * @brief Maximum address buffer size.
   * Ethereum address contains 40 hexadecimal characters hence 20 bytes.
   */
  inline const std::size_t addressBufferSize = 20;

  /**
   * @brief Maximum data buffer size. 
   * Uniswap swapExactETHForTokens requires 228 bytes.
   * Defaults to 256.
   */
  inline const std::size_t dataBufferSize = 256;

  /**
   * @brief Struct holding QUANTITY and DATA values to RLP::encode.
   * For difference between them, read here: https://eth.wiki/json-rpc/API.
   */
  struct Values {
    const char *nonce; // QUANTITY
    const char *gasPrice; // QUANTITY
    const char *gasLimit; // QUANTITY
    const char *to; // DATA
    const char *data; // DATA
    const char *value; // QUANTITY
  };

  /**
   * @brief KECCAK256 hashing function.
   * 
   * @param input input buffer
   * @param inputLength input buffer length
   * @param hash output hash buffer
   * @return output buffer length (always 32)
   */
  inline std::size_t keccak256(Utils::Buffer input, std::size_t inputLength, Utils::Buffer hash) {
    KeccakWidth1600_Sponge(1088, 512, input, inputLength, 0x01, hash, 32);
    return 32;
  }
  
  /**
   * @brief ECDSA hashing function.
   * 
   * @param hash 32 byte hash
   * @param privateKey 32 byte private key
   * @param signature output signature buffer
   * @param recid output recovery id
   * @return output signature buffer length (always 64)
   */
  inline std::size_t ecdsa(Utils::Buffer hash, Utils::Buffer privateKey, Utils::Buffer signature, int *recid) {
    secp256k1_ecdsa_recoverable_signature ecdsaSig;
    secp256k1_ecdsa_sign_recoverable(secp256k1Context, &ecdsaSig, hash, privateKey, secp256k1_nonce_function_rfc6979, NULL);
    secp256k1_ecdsa_recoverable_signature_serialize_compact(secp256k1Context, signature, recid, &ecdsaSig);
    return 64;
  }

  /**
   * @brief Signs transaction.
   * 
   * @param input input values
   * @param privateKey private key to sign with
   * @param transaction output transaction buffer
   * @return output transaction buffer length
   */
  inline std::size_t sign(Values *input, Utils::Buffer privateKey, Utils::Buffer transaction) {
    /* Create buffers holding RLP input data */

    Utils::Byte nonce[quantityBufferSize];
    Utils::Byte gasPrice[quantityBufferSize];
    Utils::Byte gasLimit[quantityBufferSize];
    Utils::Byte to[addressBufferSize];
    Utils::Byte data[dataBufferSize];
    Utils::Byte value[quantityBufferSize];
    Utils::Byte v[quantityBufferSize];
    Utils::Byte r[quantityBufferSize];
    Utils::Byte s[quantityBufferSize];

    /* Create RLP input data */

    RLP::Item rlpInput[] = {
      { .buffer = nonce, .length = 0 },
      { .buffer = gasPrice, .length = 0 },
      { .buffer = gasLimit, .length = 0 },
      { .buffer = to, .length = 0 },
      { .buffer = value, .length = 0 },
      { .buffer = data, .length = 0 },
      { .buffer = v, .length = 0 },
      { .buffer = r, .length = 0 },
      { .buffer = s, .length = 0 },
    };

    /* Assign values for first encode */

    rlpInput[0].length = Utils::hexStringToBuffer(input->nonce, rlpInput[0].buffer, true);
    rlpInput[1].length = Utils::hexStringToBuffer(input->gasPrice, rlpInput[1].buffer, true);
    rlpInput[2].length = Utils::hexStringToBuffer(input->gasLimit, rlpInput[2].buffer, true);
    rlpInput[3].length = Utils::hexStringToBuffer(input->to, rlpInput[3].buffer);
    rlpInput[4].length = Utils::hexStringToBuffer(input->value, rlpInput[4].buffer, true);
    rlpInput[5].length = Utils::hexStringToBuffer(input->data, rlpInput[5].buffer);

    rlpInput[6].buffer[0] = 0x01;
    rlpInput[6].length = 1;

    rlpInput[7].length = 0;
    rlpInput[8].length = 0;

    /* Get transaction's signature */

    std::size_t transactionLength = RLP::encodeList(rlpInput, 9, transaction);

    Utils::Byte hash[32];
    keccak256(transaction, transactionLength, hash);

    Utils::Byte signature[64];
    int recid;
    ecdsa(hash, privateKey, signature, &recid);

    /* Inject signature */

    rlpInput[6].buffer[0] = recid + 37;
    
    // Trim leading zeroes as it is not always 32 bytes
    rlpInput[7].length = 32;
    Utils::Buffer rBuffer = signature;
    while(*rBuffer == 0) {
      ++rBuffer;
      --rlpInput[7].length;
    }
    memcpy(rlpInput[7].buffer, rBuffer, rlpInput[7].length);

    // Trim leading zeroes as it is not always 32 bytes
    rlpInput[8].length = 32;
    Utils::Buffer sBuffer = signature + 32;
    while(*sBuffer == 0) {
      ++sBuffer;
      --rlpInput[8].length;
    }
    memcpy(rlpInput[8].buffer, sBuffer, rlpInput[8].length);

    /* Encode signed transaction and return buffer length */

    return RLP::encodeList(rlpInput, 9, transaction);
  }
}