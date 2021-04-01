#include <transaction.hpp>

using Transaction::ETHTransaction;

ETHTransaction::ETHTransaction() {
  // Initialize secp256k1_context
  mSecp256k1Context = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);
}

void ETHTransaction::setValues(ETHValues *transactionValues) {
  mRLP.setElement(1, transactionValues->nonce);
  mRLP.setElement(2, transactionValues->gasPrice);
  mRLP.setElement(3, transactionValues->gasLimit);
  mRLP.setElement(4, transactionValues->to, 40);
  mRLP.setElement(5, transactionValues->value);
  mRLP.setElement(6, transactionValues->data);
  mRLP.setElement(7, "1");
  mRLP.setElement(8, "");
  mRLP.setElement(9, "");
}

void ETHTransaction::getSignature(Byte privateKey[32], Byte signature[64], int *recid) {
  auto [buffer, bufferLength] = mRLP.getBuffer();

  Byte hash[32];
  KeccakWidth1600_Sponge(1088, 512, buffer, bufferLength, 0x01, hash, 32);

  secp256k1_ecdsa_recoverable_signature ecdsaSig;
  secp256k1_ecdsa_sign_recoverable(mSecp256k1Context, &ecdsaSig, hash, privateKey, secp256k1_nonce_function_rfc6979, NULL);
  secp256k1_ecdsa_recoverable_signature_serialize_compact(mSecp256k1Context, signature, recid, &ecdsaSig);
}

void ETHTransaction::sign(Byte privateKey[32]) {
  // Encode RLP
  mRLP.encode();

  // Get signature
  Byte signature[64];
  int recid;
  getSignature(privateKey, signature, &recid);

  // TODO: Requires further testings if 1 byte is sufficient for chainId = 1 (recovery id must be lower than 255 - 37)
  Byte r[1] = { static_cast<Byte>(recid + 37) };

  // Inject signature
  mRLP.setElement(7, r, 1);
  mRLP.setElement(8, signature, 32);
  mRLP.setElement(9, signature + 32, 32);

  // Encode RLP with signature
  mRLP.encode();
}

std::pair<Buffer, size_t> ETHTransaction::getBuffer() {
  return mRLP.getBuffer();
}

char *ETHTransaction::getHexString(char *output, bool nullTerminated) {
  return mRLP.getHexString(output, nullTerminated);
}