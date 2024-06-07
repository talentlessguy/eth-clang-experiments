import { privateKeyToAccount, sign } from 'https://esm.sh/viem@2.13.3/accounts'
import { type Hex, hashMessage } from 'https://esm.sh/viem@2.13.3'

const pk = Deno.env.get('PK') as Hex

const { address } = privateKeyToAccount(pk)

console.log(`Address: ${address}`)

const hash = hashMessage('Hello World')

const signature = await sign({ hash, privateKey: pk })

console.log(`\nSignature:\nr = ${signature.r}\ns = ${signature.s}\nv = ${signature.v}`)