#include <stdio.h>
#include <string.h>
#include <openssl/rsa.h>
#include <openssl/aes.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/pem.h>

#define RSA_KEY_LEN          256
#define AES_KEY_LEN          256
#define HMAC_KEY_LEN         256
#define DEV_KEY_HEADER_LEN   48

#define SW_ERROR             -1
#define SW_OK                0

int check_rsa_pub_sign(void *data_buf, int buf_len, RSA *secure_pub_key)
{
        int ret = SW_ERROR;
        SHA256_CTX sha256;
        unsigned char hash[SHA256_DIGEST_LENGTH];

        SHA256_Init(&sha256);
        SHA256_Update(&sha256, data_buf, RSA_KEY_LEN);
        SHA256_Final(hash, &sha256);

	// Get secure boot rsa pub??
	//bio = BIO_new_mem_buf(secure_pub_key, RSA_KEY_LEN);
	//rsa = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);
	
        if(RSA_verify(NID_sha256, hash, SHA256_DIGEST_LENGTH,
				data_buf + RSA_KEY_LEN,
				SHA256_DIGEST_LENGTH, secure_pub_key) == 0)
                ret = SW_OK;

        return ret;
}

int check_blob_sign(void * data_buf, int buf_len)
{
        int ret = SW_ERROR;
        SHA256_CTX sha256;
        unsigned char hash[SHA256_DIGEST_LENGTH];
	BIO *bio = NULL;
        RSA *rsa = NULL;

        SHA256_Init(&sha256);
        SHA256_Update(&sha256, data_buf + RSA_KEY_LEN + 2 * SHA256_DIGEST_LENGTH, buf_len - RSA_KEY_LEN - 2 * SHA256_DIGEST_LENGTH);
        SHA256_Final(hash, &sha256);

	// Get dev key rsa pub??
	bio = BIO_new_mem_buf(data_buf, RSA_KEY_LEN);
	rsa = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);

        if(RSA_verify(NID_sha256, hash, SHA256_DIGEST_LENGTH, data_buf + RSA_KEY_LEN + SHA256_DIGEST_LENGTH, SHA256_DIGEST_LENGTH, rsa) == 0)
                ret = SW_OK;

        return ret;
}

int decrypt_key_and_burn_key(void * data_buf, int buf_len, void * secure_aes_key, void * hmac_msg, int msg_len)
{
        int ret = SW_ERROR;
        unsigned char hmac_key[HMAC_KEY_LEN];
        unsigned int hmac_key_len = 0;
        HMAC_CTX hmac;
        AES_KEY aes;
        int enc_data_len = buf_len - RSA_KEY_LEN - 2 * SHA256_DIGEST_LENGTH - DEV_KEY_HEADER_LEN;
        unsigned char *enc_data = data_buf + RSA_KEY_LEN + 2 * SHA256_DIGEST_LENGTH + DEV_KEY_HEADER_LEN;
        unsigned char *dec_data = (unsigned char *)malloc(enc_data_len);
        SHA256_CTX sha256;
        unsigned char hash[SHA256_DIGEST_LENGTH];

        HMAC_CTX_init(&hmac);
        HMAC_Init_ex(&hmac, secure_aes_key, AES_KEY_LEN, EVP_sha256(), NULL);
        HMAC_Update(&hmac, hmac_msg, msg_len);
        HMAC_Final(&hmac, hmac_key, &hmac_key_len);
        HMAC_CTX_cleanup(&hmac);

        AES_set_decrypt_key(hmac_key, 8 * HMAC_KEY_LEN, &aes);
	int dec_len = 0;
        for(; dec_len < enc_data_len; dec_len += 16)
                AES_decrypt(enc_data + dec_len, dec_data + dec_len, &aes);

        SHA256_Init(&sha256);
        SHA256_Update(&sha256, dec_data + 16, enc_data_len - 16);
        SHA256_Final(hash, &sha256);

        if(memcmp(hash, data_buf + RSA_KEY_LEN + 2 * SHA256_DIGEST_LENGTH + 16, SHA256_DIGEST_LENGTH) == 0) {
                // burn key??
                ret = SW_OK;
        }
        free(dec_data);

        return ret;
}

int update_key(void *data_buf, int buf_len, RSA *secure_pub_key, void *secure_aes_key, void *hmac_msg, int msg_len)
{
        int ret = SW_ERROR;
        if(check_rsa_pub_sign(data_buf, buf_len, secure_pub_key) == SW_ERROR) {
		printf("check rsa pub sign failed");
                goto exit;
        }
        if(check_blob_sign(data_buf, buf_len) == SW_ERROR) {
		printf("check blob sign failed");
                goto exit;
        }
        ret = decrypt_key_and_burn_key(data_buf, buf_len, secure_aes_key, hmac_msg, msg_len);
	if(ret == SW_ERROR)
		printf("check DEC KEY failed");
exit:
        return ret;
}

int main(int argc, char * argv[])
{
	void * data_buf[1024 * 20] ={0};
	int buf_len = 0;
	FILE *buf_file = fopen("test_ota_1.pem", "rb");
	if(buf_file == NULL)
		printf("open test_ota_1.pem failed\n");
	buf_len = fread(data_buf, 1024 * 20, 1024 * 20, buf_file);
	printf("test_ota_1.pem length is %d", buf_len);
	fclose(buf_file);

	BIO *key = NULL;
        RSA *r = NULL;
        key = BIO_new(BIO_s_file());
        BIO_read_filename(key, "test_sb_rsa2048.pem");
        r = PEM_read_bio_RSAPublicKey(key, NULL, NULL, NULL);
        BIO_free_all(key);
	
	void *aes_key[256] = {0};
	FILE *aes_file = fopen("test_sb_aes256.bin", "rb");
	if(NULL == aes_file)
		printf("open test_sb_aes256.bin failed");
	int aes_len = fread(aes_key, 256, 256, aes_file);
	printf("aes key is %d", aes_len);
	fclose(aes_file);

	int ret = update_key(data_buf, buf_len, r, aes_key, NULL, 0);
	if(ret == SW_ERROR)
		 printf("update key failed");
	return 0;
}
