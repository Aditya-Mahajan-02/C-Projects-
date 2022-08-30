Public Key Cryptography

Aditya Mahajan

Purpose:
Create a set of files that will create keys to view data, similar to ssh keys. One code will be used to encrypt a file and the other shall be used by the receiver to decrypt it. These keys will be created by the keygen code and will use an RSA system to create them.


Structures: 
RSA:  Implementation of the RSA library which will create random codes for “Keygen”.
Numtheory: The implementation of the number theory functions.
RandState: Implementation of the random state interface used by the “Numtheory” and “RSA” functions.
Encrypt: Implementation of the encryption code, using the rsa key.
Decrypt: Implementation of the decrypt code, using the rsa key.
Keygen: Implementation of the key generation code using the functions created in “RSA”.



Running the code:

use "make" to format the documents

use -h with ./encrypt or ./decrypt for options

