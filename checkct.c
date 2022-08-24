#include <string.h>
#include <assert.h>

#include "csidh.h"
#include "crypto_classify.h"
#include "crypto_declassify.h"

int main()
{
  private_key priv_alice, priv_bob;
  public_key pub_alice, pub_bob;
  public_key shared_alice, shared_bob;

  csidh_private(&priv_alice);
  csidh_private(&priv_bob);

  // just in case csidh_private isn't using randombytes:
  crypto_classify(&priv_alice,sizeof priv_alice);
  crypto_classify(&priv_bob,sizeof priv_bob);

  action(&pub_alice,&base,&priv_alice);
  action(&pub_bob,&base,&priv_bob);

  // could declassify public keys at this point
  // but the action doesn't branch based on those

  action(&shared_alice,&pub_bob,&priv_alice);
  action(&shared_bob,&pub_alice,&priv_bob);

  // end of constant-time testing
  crypto_declassify(&shared_alice,sizeof shared_alice);
  crypto_declassify(&shared_bob,sizeof shared_bob);
  assert(!memcmp(&shared_alice,&shared_bob,sizeof(public_key)));

  return 0;
}
