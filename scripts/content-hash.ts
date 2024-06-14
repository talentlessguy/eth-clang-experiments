const contentHash =
  'e30101701220846a8475d7d2ee4178ea696d2f3a15075b1e57729abbfa72f01c5caf24668eec'

const ipfsHash = contentHash.slice(4)

const bytes = new Uint8Array(ipfsHash.length / 2)
for (let i = 0; i < ipfsHash.length; i += 2) {
  bytes[i / 2] = parseInt(ipfsHash.substring(i, i + 2), 16)
}

const alphabet = 'abcdefghijklmnopqrstuvwxyz234567'
let output = ''
let bits = 0
let value = 0

for (let i = 0; i < bytes.length; i++) {
  value = (value << 8) | bytes[i]
  bits += 8

  while (bits >= 5) {
    output += alphabet[(value >>> (bits - 5)) & 31]
    bits -= 5
  }
}

if (bits > 0) output += alphabet[(value << (5 - bits)) & 31]

console.log(`ipfs://${'b' + output}`)
