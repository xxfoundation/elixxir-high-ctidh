/*
 * This program shows basic usage with the highctidh csidh API.
 *
 * Compile this example program for each key size using the system installed
 * headers and linked against the installed shared library:
 *
 *  gcc -DBITS=511 -D'NAMESPACEBITS(x)=highctidh_511_##x' \
 *  -D'NAMESPACEGENERIC(x)=highctidh_##x' -o example-ctidh511 example-ctidh.c \
 *  -l highctidh_511
 *
 *  gcc -DBITS=512 -D'NAMESPACEBITS(x)=highctidh_512_##x' \
 *  -D'NAMESPACEGENERIC(x)=highctidh_##x' -o example-ctidh512 example-ctidh.c \
 *  -l highctidh_512
 *
 *  gcc -DBITS=1024 -D'NAMESPACEBITS(x)=highctidh_1024_##x' \
 *  -D'NAMESPACEGENERIC(x)=highctidh_##x' -o example-ctidh1024 example-ctidh.c \
 *  -l highctidh_1024
 *
 *  gcc -DBITS=2048 -D'NAMESPACEBITS(x)=highctidh_2048_##x' \
 *  -D'NAMESPACEGENERIC(x)=highctidh_##x' -o example-ctidh2048 example-ctidh.c \
 *  -l highctidh_2048
 *
 * */

#include <stdio.h>
#include <libhighctidh/csidh.h>

void print_hex_key(void *k, unsigned int l)
{ 
  printf("0x");
  for (unsigned int i = 0; i < l; i++)
  {
    printf("%02x", i[(unsigned char *) k]);
  }
  printf("\n");
  fflush(stdout);
}

int main()
{
  private_key sk_a, sk_b;
  public_key pk_a, pk_b;
  public_key s_a, s_b;
  bool ok = 0;

  printf("CTIDH %i vector example\n\n", BITS);
  fflush(stdout);

  printf("Generating Alice's private_key (%li bytes):\n", sizeof(private_key));
  fflush(stdout);
  csidh_private(&sk_a);
  print_hex_key(&sk_a, sizeof(private_key));
  printf("Generating Alice's public_key (%li bytes):\n", sizeof(public_key));
  fflush(stdout);
  ok = csidh(&pk_a, &base, &sk_a);
  if (!validate(&pk_a))
  {
    printf("Invalid public key:\n");
  }
  print_hex_key(&pk_a, sizeof(public_key));
  printf("Result: %i\n", ok);
  printf("\n");
  fflush(stdout);

  printf("Generating Bob's private_key (%li bytes):\n", sizeof(private_key));
  fflush(stdout);
  csidh_private(&sk_b);
  print_hex_key(&sk_b, sizeof(private_key));
  printf("Generating Bob's public_key (%li bytes):\n", sizeof(public_key));
  fflush(stdout);
  ok = csidh(&pk_b, &base, &sk_b);
  if (!validate(&pk_a))
  {
    printf("Invalid public key:\n");
  }
  print_hex_key(&pk_b, sizeof(public_key));
  printf("Result: %i\n", ok);
  printf("\n");
  fflush(stdout);

  printf("Computing DH for Alice (%li bytes):\n", sizeof(public_key));
  fflush(stdout);
  ok = csidh(&s_a, &pk_b, &sk_a);
  print_hex_key(&s_a, sizeof(public_key));
  printf("Result: %i\n", ok);
  printf("\n");
  fflush(stdout);

  printf("Computing DH for Bob (%li bytes):\n", sizeof(public_key));
  fflush(stdout);
  ok = csidh(&s_b, &pk_a, &sk_b);
  print_hex_key(&s_b, sizeof(public_key));
  printf("Result: %i\n", ok);
  printf("\n");
  fflush(stdout);

  printf("Shared keys ...");
  if (!memcmp(&s_a, &s_b, sizeof(public_key)))
  {
    printf(" match\n");
    ok = 0;
  } else {
    printf(" do not match\n");
    ok = 1;
  }
  fflush(stdout);
  return ok;
}
