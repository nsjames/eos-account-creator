<script lang="ts">
    import "$lib/app.scss";
    import { Toaster } from 'svelte-sonner'
    import GoogleAnalytics from "$lib/components/analytics/GoogleAnalytics.svelte";

    const title = 'EOS Account Creator'
    const description = 'Create EOS accounts by sending tokens from an exchange or over a bridge.'
    const url = 'https://accounts.eosnetwork.com'

</script>

<svelte:head>
    <title>{title}</title>
    <meta name="description" content={description} />
    <meta property="og:image" content="{url}/ogimage.png">
    <meta property="og:locale" content="en" />
    <meta property="og:image:width" content="1200" />
    <meta property="og:image:height" content="630" />
    <meta property="og:type" content="website" />
    <meta property="og:title" content={title} />
    <meta property="og:description" content={description} />
    <meta property="og:url" content={url} />
    <meta property="og:site_name" content={title} />
</svelte:head>

<GoogleAnalytics />

<Toaster />

<slot />
