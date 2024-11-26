<script lang="ts">
    import { slide } from "svelte/transition";

    let faq = [
        {
            question: 'Can I send more EOS?',
            answer:`
                Yes, the remaining EOS will be forwarded to your new account.
            `
        },
        {
            question: 'Where am I sending this EOS?',
            answer:`
                The EOS you send goes to a smart contract which creates your new account on the EOS Network.
                There is no middleman, and the EOS is never held by anyone.
            `
        },

    ]

    let open:number = -1;
    const toggle = (index:number) => {
        open = open === index ? -1 : index;
    }
</script>

<section class="my-10 w-full text-white">

    {#each faq as item,index}
        <section class="[&:not(:last-child)]:border-b border-zinc-700">

            <button class="w-full flex justify-between items-center cursor-pointer py-2" on:click={() => toggle(index)}>
                <span class="text-lg flex-1 text-left {open === index ? 'text-yellow-400' : ''}">
                    {item.question}
                </span>
                <span class="{open === index ? 'transform rotate-180' : ''} transition-transform duration-300">
                    <svg width="17" height="11" viewBox="0 0 17 11" fill="none" xmlns="http://www.w3.org/2000/svg">
                        <rect x="2.12134" y="0.0751953" width="12" height="3" transform="rotate(45 2.12134 0.0751953)" fill="white"/>
                        <rect x="6.29291" y="8.48529" width="12" height="3" transform="rotate(-45 6.29291 8.48529)" fill="white"/>
                    </svg>
                </span>
            </button>

            {#if open === index}
                <section class="pt-3 pb-12" transition:slide={{duration: 200}}>
                    <p class="text-sm">
                        {@html item.answer}
                    </p>
                </section>
            {/if}
        </section>
    {/each}

</section>
