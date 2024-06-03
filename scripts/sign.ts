import { privateKeyToAccount, signMessage } from 'https://esm.sh/viem@2.13.3/accounts'
import type { Hex } from 'https://esm.sh/viem@2.13.3'

const pk = Deno.env.get('PK') as Hex

const { publicKey, address } = privateKeyToAccount(pk)

console.log(`Pubkey: ${publicKey}`)
console.log(`Address: ${address}`)
console.log(`Signature: ${await signMessage({ privateKey: pk, message: 'Hello World' })}`)