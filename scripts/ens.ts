import { labelhash, namehash } from 'https://esm.sh/viem@2.13.3/ens'
import { encodeFunctionData } from 'https://esm.sh/viem@2.13.3'

console.log(`labelhash: ${labelhash('eth')}`)
const node = namehash('v1rtl.eth')
console.log(`namehash: ${node}`)

console.log(`Encoded abi: ${
  encodeFunctionData({
    functionName: 'addr',
    abi: [{
      'inputs': [
        {
          'internalType': 'bytes32',
          'name': 'node',
          'type': 'bytes32',
        },
      ],
      'name': 'addr',
      'outputs': [
        {
          'internalType': 'address payable',
          'name': '',
          'type': 'address',
        },
      ],
      'stateMutability': 'view',
      'type': 'function',
    }],
    args: [node],
  })
}`)
