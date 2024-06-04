import { privateKeyToAccount, signMessage } from 'https://esm.sh/viem@2.13.3/accounts'
import type { Hex } from 'https://esm.sh/viem@2.13.3'

const pk = Deno.env.get('PK') as Hex

const { address } = privateKeyToAccount(pk)

console.log(`Address: ${address}`)
console.log(`\nSignature: ${await signMessage({ privateKey: pk, message: 'Hello World' })}`)