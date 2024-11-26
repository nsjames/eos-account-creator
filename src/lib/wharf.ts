import {Chains, KeyType} from "@wharfkit/session"
import {APIClient, PrivateKey} from "@wharfkit/antelope"
import { ContractKit } from "@wharfkit/contract"


const chains = [
    Chains.EOS,
    Chains.Jungle4,
    Chains.KylinTestnet,
];

export default class WharfService {

    public static generateKeys(){
        const privateKey = PrivateKey.generate(KeyType.K1);
        return [
            privateKey.toWif(),
            privateKey.toPublic().toLegacyString()
        ]
    }

    public static async getCost(){
        try {
            const contractKit = new ContractKit({
                client: new APIClient(chains[0]),
            })
            const contract = await contractKit.load("openaccounts");
            const result = await contract.readonly('estimatecost');
            console.log('parseFloat(result.quantity)', parseFloat(result.quantity));
            // @ts-ignore
            return parseFloat(parseFloat(result.quantity) + 0.2).toFixed(4);
        } catch (e) {
            console.error(e);
            return 10;
        }
    }

    public static async getRamCost(){
        return WharfService.getCost();
    }

    public static isNameAvailable(name:string){
        const client = new APIClient(chains[0]);
        return client.v1.chain.get_account(name).then(() => false).catch(() => true);
    }
}
