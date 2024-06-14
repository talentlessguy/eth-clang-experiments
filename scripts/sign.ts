import { privateKeyToAccount, sign } from 'https://esm.sh/viem@2.13.3/accounts'
import {
  hashMessage,
  type Hex,
  serializeSignature,
} from 'https://esm.sh/viem@2.13.3'

const pk = Deno.env.get('PK') as Hex

const { address } = privateKeyToAccount(pk)

console.log(`Address: ${address}`)

const hash = hashMessage('Hello World')

const signature = await sign({ hash, privateKey: pk })

console.log(`\nr = ${signature.r}\ns = ${signature.s}\n`)

const hex = serializeSignature(signature)

console.log(`Signature: ${hex}`)
