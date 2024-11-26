<script lang="ts">
    import {onMount} from "svelte";
    import BackgroundOverlay from "$lib/components/BackgroundOverlay.svelte";
    import GetWallet from "$lib/components/GetWallet.svelte";
    import FAQ from "$lib/components/FAQ.svelte";
    import BlurryBg from "$lib/components/BlurryBg.svelte";
    import LogoText from "$lib/svgs/LogoText.svelte";
    import GlassBox from "$lib/components/GlassBox.svelte";
    import WharfService from "$lib/wharf";
    import LogoIcon from "$lib/svgs/LogoIcon.svelte";

    let accountName = "";
    let publicKey = "";
    let privateKey = "";
    let available = true;

    const generateKey = () => {
        const [priv, pub] = WharfService.generateKeys();
        publicKey = pub;
        privateKey = priv;
    }

    const randomName = () => {
        const chars = "12345abcdefghijklmnopqrstuvwxyz";
        let name = "";
        for (let i = 0; i < 12; i++) {
            name += chars.charAt(Math.floor(Math.random() * chars.length));
        }
        accountName = name;
        checkName();
    }

    $: accountError = (() => {
        if (accountName.length !== 12) {
            return "Account name must be 12 characters";
        }
        if(!accountName.match(/^[a-z1-5]+$/)) {
            return "Account name must be lowercase and contain only a-z and 1-5";
        }
        return "";
    })();

    const checkName = async () => {
        available = true;

        if (accountError) {
            return false;
        }

        available = await WharfService.isNameAvailable(accountName);
    }

    let ramCost = null;
    onMount(async () => {
        ramCost = await WharfService.getRamCost();
    });

    const copyText = (id:string, text: string) => {
        navigator.clipboard.writeText(text);
        const button = document.getElementById(id);
        button.innerText = "✓ Copied";
        setTimeout(() => {
            button.innerText = "Copy";
        }, 1000);
    }

    $: (() => {
        console.log('when name change', accountName);
        checkName();
    })();

</script>


<section class="bg-black w-full min-h-screen relative overflow-x-hidden px-2">
    <BackgroundOverlay />
    <BlurryBg />

    <section class="max-w-[600px] mx-auto relative mt-20">
        <a class="flex justify-center" href="https://eosnetwork.com" target="_blank" rel="noopener noreferrer">
            <section class="w-[130px]">
                <LogoIcon />
            </section>
        </a>

        <h1 class="text-white text-2xl text-center mt-4">
            Create an EOS account
        </h1>

        <GlassBox class="mt-2 overflow-hidden">
            <figure class="bg-yellow-400 opacity-90 w-[200px] aspect-[4/4] rounded-full blur-[90px] absolute -top-[180px] -right-[100px] z-0"></figure>


            <section class="flex justify-between">
                <label class="text-white text-sm ml-1">
                    Enter a name with 12 characters
                </label>
                <label class="text-white text-sm ml-1">
                    {accountName.length}/12
                </label>
            </section>
            <section class="token-input rounded-lg mt-1 flex justify-center">
                <input class="w-full text-xl outline-0 bg-transparent" placeholder="Account Name" type="text" bind:value={accountName} />
                <button class="btn ml-2" on:click={randomName}>
                    Random
                </button>
            </section>
            {#if !available && accountName.length === 12}
                <figure class="text-red-500 animate-pulse text-sm mt-1 ml-1">
                    Account name is already taken
                </figure>
            {/if}
            {#if accountName.length && !!accountError}
                <figure class="text-red-500 animate-pulse text-sm mt-1 ml-1">
                    {accountError}
                </figure>
            {/if}

            <div class="mt-5"></div>
            <label class="text-white text-sm ml-1">
                Enter a public key
            </label>
            <section class="token-input rounded-lg mt-1 flex justify-center">
                <input class="w-full text-sm outline-0 bg-transparent" placeholder="Public Key" type="text" bind:value={publicKey} />
                <!-- button to generate a new key -->
                <button class="btn ml-2" on:click={generateKey}>
                    Generate
                </button>
            </section>
            {#if privateKey.length}
                <figure class="text-white text-sm mt-1 ml-1">
                    Private Key: {privateKey}
                </figure>
            {/if}

            <div class="bg-white opacity-50 w-full h-px mt-10"></div>

            <div class="mt-5"></div>
            <!------------------------------------------------------------>

            <p class="text-white text-sm mb-6">
                Send EOS from an exchange or over a bridge to the account below with the memo provided to create your account.
            </p>
            <section class="flex justify-start gap-2 items-center">
                <button id="cost" class="btn text-xs !px-2 !py-1" on:click={() => copyText('cost', ramCost)}>
                    Copy
                </button>
                <figure class="text-white text-lg">
                    Send: <b class="font-extrabold">{ramCost} EOS</b>
                </figure>
            </section>

            <section class="flex justify-start gap-2 items-center mt-2">
                <button id="recipient" class="btn text-xs !px-2 !py-1" on:click={() => copyText('recipient', "openaccounts")}>
                    Copy
                </button>
                <figure class="text-white text-lg">
                    To: <b class="font-extrabold">openaccounts</b>
                </figure>
            </section>

            <section class="flex justify-start gap-2 items-center mt-2">
                <button id="memo" class="btn text-xs !px-2 !py-1" on:click={() => copyText('memo', accountName + ":" + publicKey)}>
                    Copy
                </button>
                <figure class="text-white text-xs overflow-x-auto">
                    Memo: <b class="font-extrabold text-[9px]">{accountName + ":" + publicKey}</b>
                </figure>
            </section>

            <!-- warning -->
            <figure class="bg-red-700 text-white text-sm px-3 py-2 mt-6 rounded">
                Make sure you send your EOS to the right account, and memo.
                This is a smart contract, if you send it the wrong key or memo, you will lose your EOS.
            </figure>

        </GlassBox>

<!--        <button class="btn mt-5" on:click={buy}>-->
<!--            Stake-->
<!--        </button>-->

        <GetWallet />

        <FAQ />

    </section>
</section>

<style lang="scss">
    .token-input {
        @apply
        p-3 pr-3 pl-5;
        border: 2px solid rgba(255, 255, 255, 0.31);
        background: rgba(0, 0, 0, 0.75);
        backdrop-filter: blur(6.800000190734863px);

        transition: border-color 0.2s;

        &:has(input:focus) {
            @apply border-yellow-400;
        }

    }
</style>
